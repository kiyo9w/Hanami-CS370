#include "../common/token.h"
#include "lexer.h"

// Add the constructor definition
Lexer::Lexer(const std::string& source) : source(source), current(0), line(1), column(1) {
    // Initialize keyword map
    initKeywords();
}

void Lexer::initKeywords(){
    // Các từ khóa Hanami
    keywords["garden"] = TokenType::GARDEN;
    keywords["species"] = TokenType::SPECIES;
    keywords["open"] = TokenType::OPEN;
    keywords["hidden"] = TokenType::HIDDEN;
    keywords["guarded"] = TokenType::GUARDED;
    keywords["grow"] = TokenType::GROW;
    keywords["blossom"] = TokenType::BLOSSOM;
    keywords["style"] = TokenType::STYLE;
    keywords["bloom"] = TokenType::BLOOM;
    keywords["water"] = TokenType::WATER;
    keywords["branch"] = TokenType::BRANCH;
    
    // Các từ khóa gốc
    keywords["else"] = TokenType::ELSE;
    keywords["while"] = TokenType::WHILE;
    keywords["for"] = TokenType::FOR;
    keywords["true"] = TokenType::TRUE;
    keywords["false"] = TokenType::FALSE;

    //Math operator
    keywords["+"] = TokenType::PLUS;
    keywords["-"] = TokenType::MINUS;
    keywords["*"] = TokenType::STAR;
    keywords["/"] = TokenType::SLASH;
    keywords["%"] = TokenType::MODULO;

    //Assign 
    keywords["="] = TokenType::ASSIGN;

    //Compare
    keywords["!="] = TokenType::NOT_EQUAL;
    keywords["<="] = TokenType::LESS_EQUAL;
    keywords[">="] = TokenType::GREATER_EQUAL;

    // Thêm vào hàm initKeywords
    keywords["::"] = TokenType::SCOPE_RESOLUTION;
    keywords["<<"] = TokenType::STREAM_OUT;
    keywords[">>"] = TokenType::STREAM_IN;
    keywords["->"] = TokenType::ARROW;
    keywords[":"] = TokenType::COLON;

    // Thêm các dấu câu khác
    keywords["("] = TokenType::LEFT_PAREN;
    keywords[")"] = TokenType::RIGHT_PAREN;
    keywords["{"] = TokenType::LEFT_BRACE;
    keywords["}"] = TokenType::RIGHT_BRACE;
    keywords["["] = TokenType::LEFT_BRACKET;
    keywords["]"] = TokenType::RIGHT_BRACKET;
    keywords[","] = TokenType::COMMA;
    keywords[";"] = TokenType::SEMICOLON;
    keywords["."] = TokenType::DOT;

}


char Lexer::advance() {
    // Kiểm tra xem đã đến cuối source chưa
    if (isEnd()) {
        return '\0';  // Hoặc throw một exception
    }
    
    char c = source[current++];
    
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    
    return c;
}



    char Lexer::peek() {
        if(isEnd()) return '\0';
        return source[current];

    }      // see the char without goto it

    char Lexer::peekNext() {
        if(current + 1 >= source.length()) return '\0';
        return source[current + 1];     // còn thằng này thì nó lại là trả về source[current+1] =]]]

    }    // see the next char from current location without goto it

    bool Lexer::isEnd() {
        return current >= source.length();
    }        // check if source is end or not

    bool Lexer::match(char expected) {
        if(isEnd()) return false;
        if(source[current] != expected) return false;

        current++;
        column++;
        return true;

    }       //check and read if match

    char Lexer::peekNextNext() {
        if (current + 2 >= source.length()) return '\0';
        return source[current + 2];
    }


    std::vector<Token> Lexer::scanTokens() {
        int callCount = 0;
        int lastPosition = -1;
        int stuckCounter = 0;
        
        try {
            while (!isEnd()) {
                callCount++;
                
                // THÊM DEBUG: Theo dõi vị trí để phát hiện lexer bị kẹt
                if (current == lastPosition) {
                    stuckCounter++;
                    std::cerr << "WARNING: Lexer possibly stuck at position " << current 
                              << " (" << stuckCounter << " times), char: '" 
                              << (current < source.length() ? source[current] : '\0') << "'" << std::endl;
                    
                    // Nếu kẹt quá nhiều lần tại cùng một vị trí, buộc phải thoát
                    if (stuckCounter > 5) {
                        std::cerr << "ERROR: Lexer definitively stuck, breaking loop" << std::endl;
                        break;
                    }
                } else {
                    stuckCounter = 0;
                }
                lastPosition = current;
                
                // THÊM DEBUG: In thông tin trước khi gọi scanToken()
                std::cerr << "--- DEBUG: scanToken() call #" << callCount 
                          << " at position: " << current 
                          << ", line: " << line 
                          << ", column: " << column << std::endl;
                
                if (!isEnd()) {
                    std::cerr << "Current char: '" << peek() 
                              << "' (ASCII: " << (int)peek() << ")" << std::endl;
                }
                
                // Gọi scanToken() và ghi log kết quả
                Token token = scanToken();
                std::cerr << "Generated token with type " << (int)token.type 
                          << " (\"" << token.lexeme << "\") at line " << token.line 
                          << ", column " << token.column << std::endl;
                
                if (token.type != TokenType::ERROR) {
                    tokens.push_back(token);
                } else {
                    tokens.push_back(token);
                    std::cerr << "Error at line " << token.line << ", column " << token.column 
                              << ": " << token.lexeme << std::endl;
                }
                
                // THÊM DEBUG: Kiểm tra vị trí sau khi xử lý token
                std::cerr << "After token: current=" << current 
                          << ", next char: '" << peek() << "'" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fatal error during lexical analysis: " << e.what() << std::endl;
        }
        
        tokens.push_back({TokenType::EOF_TOKEN, "", line, column});
        return tokens;
    }
    
    

    Token Lexer::scanToken() {
        // Dùng biến static để theo dõi vị trí và track số lượng token đã tạo
        static int previousPosition = -1;
        static int tokenCount = 0;
        static TokenType lastTokenType = TokenType::EOF_TOKEN; // Thêm biến để theo dõi loại token cuối
    

        //std::cerr << "\n--- DEBUG: scanToken() call times: " << tokenCount + 1 << " ---" << std::endl;
        //std::cerr << "Pos: " << current << ", line: " << line << ", column: " << column << std::endl;
        //std::cerr << "current char: '" << (isEnd() ? "EOF" : std::string(1, peek())) << "'" << std::endl;

        // Kiểm tra để phát hiện vòng lặp vô hạn
        if (current == previousPosition && !isEnd()) {
            std::cerr << "WARNING: Lexer not advancing at position " << current 
                      << ", line " << line << ", column " << column
                      << ", current char: '" << (isEnd() ? "EOF" : std::string(1, peek())) << "'" << std::endl;
            
            // Tiêu thụ ký tự này để tránh lặp vô hạn
            if (!isEnd()) {
                char troubleChar = advance();
                return {TokenType::ERROR, std::string(1, troubleChar), line, column - 1};
            }
        }
        
        // Kiểm tra giới hạn token để tránh vòng lặp vô hạn
        tokenCount++;
        //std::cerr<<peek()<<std::endl;
        const int MAX_TOKENS = 5000; // Số lượng token tối đa hợp lý
        if (tokenCount > MAX_TOKENS) {
            std::cerr<<peek() << "here !!!!!!" <<std::endl;
            return {TokenType::ERROR, "Too many tokens generated (possible infinite loop)", line, column};
        }
        
        previousPosition = current;
            
        // Bỏ qua khoảng trắng
        skipWhitespace();

        // Check if we're at the end of file
        if (isEnd()) {
            return {TokenType::EOF_TOKEN, "", line, column};
        }
    
        // Lưu vị trí bắt đầu token
        int startColumn = column;
        int startLine = line;
        
        // Đọc ký tự đầu tiên
        char c = advance();
    
        // Xử lý chuỗi
        if (c == '"') {
            return string();
        }
    
        // Xử lý số
        if (isdigit(c)) {
            current--; // Lùi lại để đọc từ đầu
            column--;
            return Number();
        }
        
        // Xử lý số bắt đầu bằng dấu chấm (.5)
        if (c == '.' && isdigit(peek())) {
            current--; // Lùi lại để Number() đọc cả dấu chấm
            column--;
            return Number();
        }
        
        // Xử lý identifiers và keywords
        if (isalpha(c) || c == '_') {
            current--;  // Move back to re-read in identifier()
            column--;   // Adjust column position as well
            Token token = identifier();
            lastTokenType = token.type; // Lưu lại token type
            return token;
        }
        
        // Xử lý các toán tử và dấu câu
        switch (c) {
            case '(': {
                Token token = {TokenType::LEFT_PAREN, "(", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case ')': {
                Token token = {TokenType::RIGHT_PAREN, ")", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case '{': {
                Token token = {TokenType::LEFT_BRACE, "{", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case '}': {
                Token token = {TokenType::RIGHT_BRACE, "}", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case '[': {
                Token token = {TokenType::LEFT_BRACKET, "[", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case ']': {
                Token token = {TokenType::RIGHT_BRACKET, "]", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case ',': {
                Token token = {TokenType::COMMA, ",", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case '.': {
                Token token = {TokenType::DOT, ".", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case ';': {
                Token token = {TokenType::SEMICOLON, ";", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case '+': {
                Token token = {TokenType::PLUS, "+", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            case '*': {
                Token token = {TokenType::STAR, "*", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            case ':': {
                //std::cerr << "DEBUG: Detected ':' at line " << startLine << ", column " << startColumn << std::endl;
                if (peek() == ':') {
                    //std::cerr << "DEBUG: Found '::' at line " << startLine << ", column " << startColumn << std::endl;
                    advance(); // Tiêu thụ ':'
                    //std::cerr << "DEBUG: Found SCOPE_RESOLUTION, current position after consuming: " << current 
                 //<< ", next char: '" << peek() << "'" << std::endl;
                    Token token = {TokenType::SCOPE_RESOLUTION, "::", startLine, startColumn};
                    lastTokenType = token.type;
                    //advance(); // Tiêu thụ ':'
                    return token;
                }
                Token token = {TokenType::COLON, ":", startLine, startColumn};
                lastTokenType = token.type;
                //advance(); // Tiêu thụ ':'
                return token;
            }
                
            case '-': {
                if (peek() == '>') {
                    advance(); // Tiêu thụ '>'
                    //std::cerr << "DEBUG: Consumed ARROW at line " << startLine << ", column " << startColumn 
                            //<< ", next char is '" << peek() << "'" << std::endl;
                    Token token = {TokenType::ARROW, "->", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                }
                Token token = {TokenType::MINUS, "-", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            case '=': {
                if (peek() == '=') {
                    advance(); // Tiêu thụ '='
                    Token token = {TokenType::EQUAL, "==", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                }
                Token token = {TokenType::ASSIGN, "=", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            case '!': {
                if (peek() == '=') {
                    advance(); // Tiêu thụ '='
                    Token token = {TokenType::NOT_EQUAL, "!=", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                }
                Token token = {TokenType::NOT, "!", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            case '<': {
                // Xử lý đặc biệt cho style includes
                if (lastTokenType == TokenType::STYLE) {
                    std::string includePath;
                    while (!isEnd() && peek() != '>') {
                        includePath += advance();
                    }
                    
                    if (isEnd()) {
                        Token token = {TokenType::ERROR, "Unterminated include directive", startLine, startColumn};
                        lastTokenType = token.type;
                        return token;
                    }
                    
                    // Tiêu thụ '>'
                    advance();
                    
                    Token token = {TokenType::STYLE_INCLUDE, includePath, startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                }
                
                // Xử lý thông thường
                if (peek() == '<') {
                    advance(); // Tiêu thụ '<'
                    Token token = {TokenType::STREAM_OUT, "<<", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                } else if (peek() == '=') {
                    advance(); // Tiêu thụ '='
                    Token token = {TokenType::LESS_EQUAL, "<=", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                }
                Token token = {TokenType::LESS, "<", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            case '>': {
                if (peek() == '>') {
                    advance(); // Tiêu thụ '>'
                    Token token = {TokenType::STREAM_IN, ">>", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                } else if (peek() == '=') {
                    advance(); // Tiêu thụ '='
                    Token token = {TokenType::GREATER_EQUAL, ">=", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                }
                Token token = {TokenType::GREATER, ">", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            case '&': {
                if (peek() == '&') {
                    advance(); // Tiêu thụ '&'
                    Token token = {TokenType::AND, "&&", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                }
                Token token = {TokenType::ERROR, "&", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            case '|': {
                if (peek() == '|') {
                    advance(); // Tiêu thụ '|'
                    Token token = {TokenType::OR, "||", startLine, startColumn};
                    lastTokenType = token.type;
                    return token;
                }
                Token token = {TokenType::ERROR, "|", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
            
            case '/': {
                if (peek() == '/') {
                    // Đọc qua toàn bộ comment
                    while (!isEnd() && peek() != '\n') {
                        advance();
                    }
                    // Thử lấy token tiếp theo, thay vì trả về comment token
                    return scanToken();
                }
                Token token = {TokenType::SLASH, "/", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            case '%': {
                Token token = {TokenType::MODULO, "%", startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
    
            default: {
                //advance(); // Tiêu thụ ký tự không hợp lệ
                //std::cerr << "DEBUG: Invalid character '" << c << "' at line " 
                          //<< startLine << ", column " << startColumn << std::endl;
                Token token = {TokenType::ERROR, std::string(1, c), startLine, startColumn};
                lastTokenType = token.type;
                return token;
            }
        }
    }
    
    
    

    Token Lexer::identifier() {
        int startLine = line;
        int startColumn = column;
        
        //std::cerr << "DEBUG: identifier() start at: " << current << std::endl;
        
        std::string lexeme;
        
        // Đọc bắt buộc ký tự đầu tiên
        if (!isEnd()) {
            lexeme += advance();
            //std::cerr << "DEBUG: First char: '" << lexeme << "'" << std::endl;
        }
        
        // Đọc các ký tự còn lại
        while (!isEnd() && (isalnum(peek()) || peek() == '_')) {
            lexeme += advance();
        }
        
        // Kiểm tra từ khóa sử dụng map thay vì nhiều if-else
        auto it = keywords.find(lexeme);
        if (it != keywords.end()) {
            //std::cerr << "DEBUG: identifier() end at: " << current << std::endl;
            return {it->second, lexeme, startLine, startColumn};
        }
        
        // Nếu không phải từ khóa, return as identifier
        //std::cerr << "DEBUG: identifier() end at: " << current << std::endl;
        return {TokenType::IDENTIFIER, lexeme, startLine, startColumn};
    }
    
    
    
    
    Token Lexer::Number() {
        int startColumn = column;
        std::string lexeme = "";
        
        // Lưu vị trí ban đầu để kiểm tra tiến triển
        int startPos = current;
        
        // 1. XỬ LÝ DẤU +/- ĐỨNG ĐẦU
        bool hasLeadingSign = false;
        if (peek() == '-' || peek() == '+') {
            // Kiểm tra xem đây là dấu đơn lẻ hay một phần của số
            if (!isdigit(peekNext()) && peekNext() != '.') {
                char op = advance();
                TokenType type = (op == '+') ? TokenType::PLUS : TokenType::MINUS;
                return {type, std::string(1, op), line, startColumn};
            }
            
            lexeme += advance();
            hasLeadingSign = true;
        }
        
        // 2. XỬ LÝ SỐ BẮT ĐẦU BẰNG DẤU CHẤM (.123)
        if (peek() == '.') {
            lexeme += advance();  // Tiêu thụ dấu chấm
            
            // Nếu không có chữ số nào sau dấu chấm và không có chữ số nào trước đó
            if (!isdigit(peek()) && (lexeme == "." || lexeme == "+." || lexeme == "-.")) {
                if (lexeme == ".") {
                    // Đảm bảo đã tiêu thụ ít nhất một ký tự
                    if (current > startPos) {
                        return {TokenType::DOT, ".", line, startColumn};
                    } else {
                        // Nếu không tiêu thụ gì, tiêu thụ dấu chấm
                        advance();
                        return {TokenType::DOT, ".", line, startColumn};
                    }
                }
                
                // Trả về dấu +/- nếu chỉ có +. hoặc -.
                char op = lexeme[0];
                // Đảm bảo đã tiêu thụ ít nhất một ký tự
                if (current > startPos) {
                    return {op == '+' ? TokenType::PLUS : TokenType::MINUS, std::string(1, op), line, startColumn};
                } else {
                    // Nếu không tiêu thụ gì, tiêu thụ một ký tự
                    advance();
                    return {op == '+' ? TokenType::PLUS : TokenType::MINUS, std::string(1, op), line, startColumn};
                }
            }
            
            // Đọc chữ số sau dấu chấm thập phân
            consumeDigits(lexeme);
            
            // Xử lý ký hiệu mũ (1.23e45)
            handleExponent(lexeme, false);
            
            // Xử lý hậu tố kiểu dữ liệu (f, L, u, v.v.)
            handleTypeSuffix(lexeme);
            
            // Kiểm tra xem có tiêu thụ ký tự nào không
            if (current > startPos) {
                return {TokenType::NUMBER, lexeme, line, startColumn};
            } else {
                // Nếu không tiêu thụ gì, tiêu thụ một ký tự và trả về lỗi
                advance();
                return {TokenType::ERROR, "Invalid number format", line, startColumn};
            }
        }
        
        // 3. XỬ LÝ SỐ THẬP PHÂN THÔNG THƯỜNG VÀ SỐ ĐẶC BIỆT (HEX, BINARY, OCTAL)
        else if (isdigit(peek())) {
            // Đọc phần nguyên
            consumeDigits(lexeme);
            
            // KIỂM TRA SỐ ĐẶC BIỆT (0x, 0b, 0o)
            if (lexeme == "0" && (peek() == 'x' || peek() == 'X' || 
                               peek() == 'b' || peek() == 'B' || 
                               peek() == 'o' || peek() == 'O')) {
                Token specialToken = handleSpecialNumber(lexeme, startColumn);
                
                // Kiểm tra xem có tiêu thụ ký tự nào không
                if (current > startPos) {
                    return specialToken;
                } else {
                    // Nếu không tiêu thụ gì, tiêu thụ một ký tự và trả về lỗi
                    advance();
                    return {TokenType::ERROR, "Invalid special number format", line, startColumn};
                }
            }
            
            // XỬ LÝ TRƯỜNG HỢP DẤU CHẤM ĐÔI (1..)
            if (peek() == '.') {
                lexeme += advance();  // Tiêu thụ dấu chấm đầu tiên
                
                if (peek() == '.') {
                    // Đây là trường hợp 1.. - không tiêu thụ dấu chấm thứ hai
                    // Kiểm tra xem có tiêu thụ ký tự nào không
                    if (current > startPos) {
                        return {TokenType::NUMBER, lexeme, line, startColumn};
                    } else {
                        // Nếu không tiêu thụ gì, tiêu thụ một ký tự và trả về lỗi
                        advance();
                        return {TokenType::ERROR, "Invalid number format", line, startColumn};
                    }
                }
                
                // Tiếp tục xử lý như số thập phân bình thường
                consumeDigits(lexeme);
            }
            
            // Xử lý ký hiệu mũ (123e45)
            handleExponent(lexeme, false);
            
            // Xử lý hậu tố kiểu dữ liệu
            handleTypeSuffix(lexeme);
            
            // Kiểm tra xem có tiêu thụ ký tự nào không
            if (current > startPos) {
                return {TokenType::NUMBER, lexeme, line, startColumn};
            } else {
                // Nếu không tiêu thụ gì, tiêu thụ một ký tự và trả về lỗi
                advance();
                return {TokenType::ERROR, "Invalid number format", line, startColumn};
            }
        }
        
        // 4. XỬ LÝ DẤU +/- ĐƠN ĐỘC
        else if (hasLeadingSign) {
            char op = lexeme[0];
            // Kiểm tra xem có tiêu thụ ký tự nào không
            if (current > startPos) {
                return {op == '+' ? TokenType::PLUS : TokenType::MINUS, lexeme, line, startColumn};
            } else {
                // Nếu không tiêu thụ gì, tiêu thụ một ký tự và trả về token dấu
                advance();
                return {op == '+' ? TokenType::PLUS : TokenType::MINUS, std::string(1, op), line, startColumn};
            }
        }
        
        // 5. TRƯỜNG HỢP KHÔNG HỢP LỆ
        // Đảm bảo luôn tiêu thụ ít nhất một ký tự để tránh lặp vô hạn
        if (current == startPos && !isEnd()) {
            advance(); // Tiêu thụ một ký tự để tránh lặp vô hạn
        }
        
        return {TokenType::ERROR, "Invalid number", line, startColumn};
    }



    // Các hàm phụ trợ    
    void Lexer::consumeDigits(std::string& lexeme) {
        while (isdigit(peek()) || (peek() == '\'' && isdigit(peekNext()))) {
            lexeme += advance();
        }
    }
    
    void Lexer::consumeHexDigits(std::string& lexeme) {
        while (isxdigit(peek()) || (peek() == '\'' && isxdigit(peekNext()))) {
            lexeme += advance();
        }
    }
    
    bool Lexer::consumeBinaryDigits(std::string& lexeme) {
        bool hasDigits = false;
        while ((peek() == '0' || peek() == '1') || 
               (peek() == '\'' && (peekNext() == '0' || peekNext() == '1'))) {
            hasDigits = true;
            lexeme += advance();
        }
        return hasDigits;
    }
    
    bool Lexer::consumeOctalDigits(std::string& lexeme) {
        bool hasDigits = false;
        while ((peek() >= '0' && peek() <= '7') || 
               (peek() == '\'' && peekNext() >= '0' && peekNext() <= '7')) {
            hasDigits = true;
            lexeme += advance();
        }
        return hasDigits;
    }
    
    void Lexer::handleExponent(std::string& lexeme, bool isHexFloat) {
        char expChar = isHexFloat ? 'p' : 'e';
        char expCharUpper = isHexFloat ? 'P' : 'E';
        
        if ((peek() == expChar || peek() == expCharUpper) && 
            (isdigit(peekNext()) || peekNext() == '+' || peekNext() == '-' || 
             (peekNext() == '\'' && isdigit(peekNextNext())))) {
            lexeme += advance();  // Tiêu thụ e/E hoặc p/P
            
            // Xử lý dấu sau e/p
            if (peek() == '+' || peek() == '-') {
                lexeme += advance();
            }
            
            // Phải có ít nhất một chữ số sau e/p
            bool hasExponentDigits = false;
            while (isdigit(peek()) || (peek() == '\'' && isdigit(peekNext()))) {
                hasExponentDigits = true;
                lexeme += advance();
            }
            
            // Nếu không có chữ số sau e/p, quay lại trạng thái trước đó
            if (!hasExponentDigits) {
                size_t lastExpPos = lexeme.find_last_of(isHexFloat ? "pP" : "eE");
                if (lastExpPos != std::string::npos) {
                    lexeme = lexeme.substr(0, lastExpPos);
                }
            }
        }
    }
    
    void Lexer::handleTypeSuffix(std::string& lexeme) {
        // Float/double suffixes (f, F)
        if (peek() == 'f' || peek() == 'F') {
            lexeme += advance();
        } 
        // Long/Long Long suffixes (l, L, ll, LL)
        else if (peek() == 'l' || peek() == 'L') {
            lexeme += advance();
            
            // Kiểm tra "long long" (ll, LL)
            if (peek() == 'l' || peek() == 'L') {
                lexeme += advance();
            }
            
            // Hậu tố unsigned sau long/long long (ul, uL, Ul, UL, ull, uLL, etc.)
            if (peek() == 'u' || peek() == 'U') {
                lexeme += advance();
            }
        }
        // Unsigned suffixes (u, U) 
        else if (peek() == 'u' || peek() == 'U') {
            lexeme += advance();
            
            // Long/long long sau unsigned (ul, uL, ull, uLL)
            if (peek() == 'l' || peek() == 'L') {
                lexeme += advance();
                
                // Second l/L for long long
                if (peek() == 'l' || peek() == 'L') {
                    lexeme += advance();
                }
            }
        }
    }
    
    Token Lexer::handleSpecialNumber(std::string& lexeme, int startColumn) {
        char prefix = advance();
        lexeme += prefix;
        
        // XỬ LÝ SỐ THẬP LỤC PHÂN (0x...)
        if (prefix == 'x' || prefix == 'X') {
            bool hasDigits = false;
            
            // Đọc các chữ số thập lục phân
            consumeHexDigits(lexeme);
            
            // Kiểm tra xem có chữ số thập lục phân nào không
            hasDigits = (lexeme.size() > 2);  // 2 = "0x"
            
            // Xử lý số thập lục phân dạng float (0x1.Ap3)
            if (peek() == '.') {
                lexeme += advance();  // Tiêu thụ dấu chấm
                
                // Đọc các chữ số thập lục phân sau dấu chấm
                bool hasDigitsAfterDot = false;
                std::string beforeConsuming = lexeme;
                consumeHexDigits(lexeme);
                hasDigitsAfterDot = (lexeme != beforeConsuming);
                
                // Cần có ít nhất một chữ số trước hoặc sau dấu chấm
                hasDigits = hasDigits || hasDigitsAfterDot;
            }
            
            // Xử lý phần mũ của số thập lục phân dạng float (0x1.Ap3)
            if (hasDigits) {
                handleExponent(lexeme, true);  // true = hex float
            }
            
            // Nếu không tìm thấy chữ số thập lục phân nào
            if (!hasDigits) {
                return {TokenType::NUMBER, "0", line, startColumn};
            }
        }
        // XỬ LÝ SỐ NHỊ PHÂN (0b...)
        else if (prefix == 'b' || prefix == 'B') {
            bool hasDigits = consumeBinaryDigits(lexeme);
            
            // Không có chữ số nhị phân
            if (!hasDigits) {
                return {TokenType::NUMBER, "0", line, startColumn};
            }
        }
        // XỬ LÝ SỐ BÁT PHÂN (0o...)
        else if (prefix == 'o' || prefix == 'O') {
            bool hasDigits = consumeOctalDigits(lexeme);
            
            // Không có chữ số bát phân
            if (!hasDigits) {
                return {TokenType::NUMBER, "0", line, startColumn};
            }
        }
        
        // Xử lý hậu tố kiểu dữ liệu cho các số đặc biệt
        handleTypeSuffix(lexeme);
        
        return {TokenType::NUMBER, lexeme, line, startColumn};
    }
    

    
    
    Token Lexer::string() {
        int startColumn = column - 1; // Trừ 1 vì dấu ngoặc kép đã được tiêu thụ
        int startLine = line;
        std::string lexeme;
        
        // Đọc đến dấu ngoặc kép đóng
        while (!isEnd() && peek() != '"') {
            char c = advance();
            lexeme += c;
            
            // Xử lý ký tự đặc biệt và xuống dòng
            if (c == '\\' && !isEnd()) {
                lexeme += advance();
            } else if (c == '\n') {
                line++;
                column = 1;
            }
        }
        
        // Kiểm tra kết thúc chuỗi
        if (isEnd()) {
            return {TokenType::ERROR, "Unterminated string", startLine, startColumn};
        }
        
        // Tiêu thụ dấu ngoặc kép đóng
        advance();
        
        // Trả về token STRING
        return {TokenType::STRING, lexeme, startLine, startColumn};
    }
    

    void Lexer::skipWhitespace() {
        while (!isEnd()) {
            char c = peek();
            
            if (c == ' ' || c == '\t' || c == '\r') {
                // Bỏ qua các khoảng trắng thông thường
                advance();
            } else if (c == '\n') {
                // Xử lý ký tự xuống dòng
                advance();
                line++;
                column = 1; // Đặt cột về 1 cho dòng mới
            } else {
                // Dừng lại khi gặp ký tự không phải khoảng trắng
                break;
            }
        }
    }
    