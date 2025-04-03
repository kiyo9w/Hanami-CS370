#include "lexer.h"

char Lexer::advance() {
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



    std::vector<Token> Lexer::scanTokens() {
        try {
            while (!isEnd()) {
                Token token = scanToken();
                if (token.type != TokenType::ERROR) {
                    tokens.push_back(token);
                } else {
                    // Xử lý lỗi - đã được thêm trong skipComment() hoặc có thể thêm ở đây
                    tokens.push_back(token);
                    // Có thể ghi log hoặc báo cáo lỗi
                    std::cerr << "Error at line " << token.line << ", column " << token.column 
                              << ": " << token.lexeme << std::endl;
                }
            }
        } catch (const std::exception& e) {
            // Bắt các ngoại lệ không xử lý được
            std::cerr << "Fatal error during lexical analysis: " << e.what() << std::endl;
        }
        
        tokens.push_back({TokenType::EOF_TOKEN, "", line, column});
        return tokens;
    }

    Token Lexer::scanToken() {
        while (!isEnd()) {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\r') {
                advance();
            } else if (c == '\n') {
                advance();
                line++;
                column = 1; // Đặt cột về 1 cho dòng mới
            } else {
                break; // Thoát khỏi vòng lặp nếu không phải khoảng trắng
            }
        }

    //check if the token is end or not
    if (isEnd()) {
        return {TokenType::EOF_TOKEN, "", line, column};
    }

    // Lưu vị trí bắt đầu token trước khi đọc ký tự
    int startColumn = column;
    int startLine = line;
    char c = advance();

    // Xử lý comments
    if (c == '/' && peek() == '/') {
        // 1 line comment

        advance();
        std::string commentText = "// ";
        while (peek() != '\n' && !isEnd()) { 
            commentText += advance();
        }
        // Tạo token COMMENT thay vì bỏ qua
        return {TokenType::COMMENT, commentText, startLine, startColumn};
    }
    
    if (c == '/' && peek() == '*') {
        // many line comments
        Token commentToken = skipComment();
        return commentToken;
    }

    // Xử lý các token khác
    switch (c) {
        // Dấu câu đơn
        case '(': return {TokenType::LEFT_PAREN, "(", line, startColumn};
        case ')': return {TokenType::RIGHT_PAREN, ")", line, startColumn}; 
        case '{': return {TokenType::LEFT_BRACE, "{", line, startColumn};
        case '}': return {TokenType::RIGHT_BRACE, "}", line, startColumn};
        case '[': return {TokenType::LEFT_BRACKET, "[", line, startColumn};
        case ']': return {TokenType::RIGHT_BRACKET, "]", line, startColumn};
        case ',': return {TokenType::COMMA, ",", line, startColumn};
        case '.': return {TokenType::DOT, ".", line, startColumn};
        case ';': return {TokenType::SEMICOLON, ";", line, startColumn};
        case '+': return {TokenType::PLUS, "+", line, startColumn};
        case '-': return {TokenType::MINUS, "-", line, startColumn};
        case '*': return {TokenType::MULTIPLY, "*", line, startColumn};
        case '/': return {TokenType::DIVIDE, "/", line, startColumn};
            
        
        // Dấu câu 2 ký tự
        case '=':
            if (match('=')) return {TokenType::EQUAL, "==", line, startColumn};
            else return {TokenType::ASSIGN, "=", line, startColumn};
        case '!':
            if (match('=')) return {TokenType::NOT_EQUAL, "!=", line, startColumn};
            else return {TokenType::NOT, "!", line, startColumn};
        case '<':
            if (match('=')) return {TokenType::LESS, "<=", line, startColumn}; // Sửa LESS thành LESS_EQUAL
            else return {TokenType::LESS, "<", line, startColumn};
        case '>':
            if (match('=')) return {TokenType::GREATER, ">=", line, startColumn}; // Sửa GREATER thành GREATER_EQUAL
            else return {TokenType::GREATER, ">", line, startColumn};
        case '&':
            if (match('&')) return {TokenType::AND, "&&", line, startColumn};
            else return {TokenType::ERROR, "&", line, startColumn};
        case '|':
            if (match('|')) return {TokenType::OR, "||", line, startColumn};
            else return {TokenType::ERROR, "|", line, startColumn};
        
        case '"': return string(); // Xử lý chuỗi

        default:
            if (isdigit(c)) {
                // Lùi lại một ký tự để xử lý số
                current--;
                column--;
                return Number(); // Sửa Number() thành number() cho nhất quán
            } else if (isalpha(c) || c == '_') {
                // Lùi lại một ký tự để xử lý identifier
                current--;
                column--;
                return identifier();
            } else {
                // Ký tự không nhận dạng được
                return {TokenType::ERROR, std::string(1, c), line, startColumn};
            }
        }
    }   // scan ONE token


    Token Lexer::identifier() {
        int startColumn = column;

        //read the first char
        advance();

        //read the valid chars for identifier
        while(isalnum(peek()) || peek() == '_'){
            advance();
        }

        //take the lexeme
        int length = column - startColumn;
        std::string lexeme = source.substr(current - length, length);


        //check whatever is a keyword or not
        //actually, we need a hashtable here, but i'm busy fking trung rn
        if (lexeme == "if") return {TokenType::IF, lexeme, line, startColumn};
        if (lexeme == "else") return {TokenType::ELSE, lexeme, line, startColumn};
        if (lexeme == "while") return {TokenType::WHILE, lexeme, line, startColumn};
        if (lexeme == "for") return {TokenType::FOR, lexeme, line, startColumn};
        if (lexeme == "return") return {TokenType::RETURN, lexeme, line, startColumn};
        if (lexeme == "function") return {TokenType::FUNCTION, lexeme, line, startColumn};
        if (lexeme == "true") return {TokenType::TRUE, lexeme, line, startColumn};
        if (lexeme == "false") return {TokenType::FALSE, lexeme, line, startColumn};

        //otherwise, it's a indentifier
        return {TokenType::IDENTIFIER, lexeme, line, startColumn};

    }     // process identifier and keywords

    Token Lexer::Number() {
        int startColumn = column;

        //read the integer number
        while(isdigit(peek())){
            advance();
        }

        //check if it's the real number or not
        if(peek() == '.' && isdigit(peekNext())){
            //read the comma
            advance();

            //read the number after the dot
            while(isdigit(peek())){
                advance();
            }
        }


        //take the lexeme
        int length = column - startColumn;
        std::string lexeme = source.substr(current - length, length);

        return {TokenType::NUMBER, lexeme, line, startColumn};
    }         //process number

    Token Lexer::string() {
        int startColumn = column - 1; // because we read the " already

        //read
        while(peek() != '"' && !isEnd()){
            if(peek() == '\n'){
                line++;
                column = 1;
            }
            advance();
        }

        //check if the string is correct or not
        if(isEnd()){
            return {TokenType::ERROR, "Unterminated string", line, startColumn};
        }

        //read the " left
        advance();

        //take the lexeme
        int length = column - startColumn - 1;
        std::string lexeme = source.substr(current - length, length - 1);

        return {TokenType::STRING, lexeme, line, startColumn};
    }         //process string

    Token Lexer::skipComment() {
        // We already read the /* at first
        int startLine = line;
        int startColumn = column - 2; // -2 vì đã đọc "/*"
    
        // Continue until see the */
        while(!isEnd()) {
            if(peek() == '*' && peekNext() == '/') {
                // Read
                advance();
                advance();
                
                // Return a COMMENT token instead of empty return
                return {
                    TokenType::COMMENT,
                    "/* ... */",
                    startLine,
                    startColumn
                };
            }
    
            if(peek() == '\n') {
                line++;
                column = 1;
            }
    
            advance();
        }
    
        // If no found the */, return error
        return {
            TokenType::ERROR,
            "Unterminated comment starting at line " + std::to_string(startLine) + 
            ", column " + std::to_string(startColumn),
            startLine,
            startColumn
        };
    }
    