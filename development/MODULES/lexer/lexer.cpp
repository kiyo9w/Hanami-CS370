#include "../common/token.h"
#include "../common/utils.h"
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
        if(source[current + 1] != expected) return false;

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
                std::cerr << "Generated token with type " << tokenTypeToString(token.type) 
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
        static int previousPosition = -1;
        // static int tokenCount = 0; // Keep for potential future debugging

        // Prevent infinite loops
        if (current == previousPosition && !isEnd()) {
            std::cerr << "WARNING: Lexer stuck at position " << current 
                      << ", line " << line << ", col " << column 
                      << ", char: '" << peek() << "'" << std::endl;
            char stuckChar = advance(); // Consume the problematic character
            return {TokenType::ERROR, std::string("Lexer stuck on character: ") + stuckChar, line, column - 1};
        }
        previousPosition = current;
            
        skipWhitespace();

        if (isEnd()) {
            return {TokenType::EOF_TOKEN, "", line, column};
        }
    
        int startColumn = column;
        int startLine = line; // Capture start line for tokens
        char c = peek();

        // --- Handle Comments First --- 
        if (c == '/' && peekNext() == '/') {
            while (peek() != '\n' && !isEnd()) advance();
            return scanToken(); // Recursively call to get the next actual token
        }
        if (c == '/' && peekNext() == '*') {
            advance(); advance(); // Consume /*
            int commentStartLine = line;
            int commentStartCol = startColumn;
            while (!isEnd()) {
                if (peek() == '*' && peekNext() == '/') {
                    advance(); advance(); // Consume */
                    break;
            }
                if (isEnd()) { // Unterminated comment
                     return {TokenType::ERROR, "Unterminated block comment", commentStartLine, commentStartCol};
                }
                advance(); // Consume comment character
            }
            return scanToken(); // Get next actual token
        }
        
        // --- Handle STYLE Include --- 
        // Check for 'style' keyword specifically
        if (c == 's' && source.substr(current, 5) == "style") {
             int keywordEndPos = current + 5;
             // Check if it's followed by non-alphanumeric to confirm it's the keyword
             if (keywordEndPos >= source.length() || !isalnum(source[keywordEndPos])) {
                 // Consume "style"
                 current = keywordEndPos;
                 column += 5;
                 skipWhitespace(); // Skip space before < or "
                 startColumn = column; // Update start column for the path
                 char pathDelimiter = peek();
                 std::string pathLexeme;
                 
                 if (pathDelimiter == '<') {
                     advance(); // Consume '<'
                     while (!isEnd() && peek() != '>' && peek() != '\n') {
                         pathLexeme += advance();
                     }
                if (peek() == '>') {
                         advance(); // Consume '>'
                         return {TokenType::STYLE_INCLUDE, pathLexeme, startLine, startColumn};
                     } else {
                         return {TokenType::ERROR, "Unterminated style path starting with <", startLine, startColumn};
                     }
                 } else if (pathDelimiter == '"') {
                      advance(); // Consume '"'
                      int pathStartLine = line; // Store line in case of multi-line path
                      while (!isEnd() && peek() != '"' && peek() != '\n') {
                         pathLexeme += advance();
                      }
                       if (peek() == '"') {
                         advance(); // Consume '"'
                         return {TokenType::STYLE_INCLUDE, pathLexeme, pathStartLine, startColumn};
                     } else {
                         return {TokenType::ERROR, "Unterminated style path starting with \"", pathStartLine, startColumn};
                     }
                 } else {
                      // It was "style" but not followed by < or ", treat as identifier or error? 
                      // For now, let it fall through to identifier handling if possible, 
                      // or return error if context demands include path.
                      // Let's return error for clarity, parser expects path here.
                      return {TokenType::ERROR, "Expected '<' or '\"' after style keyword", startLine, column};
                 }
             }
             // If it wasn't "style " or similar, fall through to general identifier handling
        }

        // --- Handle Multi-Character Operators & Punctuation FIRST --- 
        // Order matters: check longer tokens before shorter prefixes
        if (c == '<') {
             if (match('<')) {advance(); return {TokenType::STREAM_OUT, "<<", startLine, startColumn};}
             if (match('=')) {advance(); return {TokenType::LESS_EQUAL, "<=", startLine, startColumn};}
             // If neither match succeeded, consume c and return LESS
             advance();
             return {TokenType::LESS, "<", startLine, startColumn};
        }
        if (c == '>') {
             if (match('>')) {advance(); return {TokenType::STREAM_IN, ">>", startLine, startColumn};}
             if (match('=')) {advance(); return {TokenType::GREATER_EQUAL, ">=", startLine, startColumn};}
             // If neither match succeeded, consume c and return GREATER
                    advance();
             return {TokenType::GREATER, ">", startLine, startColumn};
        }
        if (c == '=') {
             if (match('=')) {advance(); return {TokenType::EQUAL, "==", startLine, startColumn};}
              // If not '==', consume the original '=' and return ASSIGN
             advance(); // Consume the '='
             return {TokenType::ASSIGN, "=", startLine, startColumn};
        }
         if (c == '!') {
             if (match('=')) {advance(); return {TokenType::NOT_EQUAL, "!=", startLine, startColumn};}
             // If not '!=', consume c and return NOT
             advance();
             return {TokenType::NOT, "!", startLine, startColumn};
        }
        if (c == '-') {
             if (match('>')) {advance(); return {TokenType::ARROW, "->", startLine, startColumn};}
             // Check if it's a negative number (BEFORE treating as minus operator)
             if (isdigit(peek()) || (peek() == '.' && isdigit(peekNext()))) {
                 return Number(); // Let Number() handle it, including the leading '-'
             }
             // If not '->' or start of number, consume c and return MINUS
             advance();
             return {TokenType::MINUS, "-", startLine, startColumn};
        }
        if (c == ':') {
            if (match(':')) {advance(); return {TokenType::SCOPE_RESOLUTION, "::", startLine, startColumn};}
             // If not '::', consume c and return COLON
            advance();
            return {TokenType::COLON, ":", startLine, startColumn};
        }

        // --- Check for Start of Number ---
        if (isdigit(c) || (c == '.' && isdigit(peek()))) // Check current char or '.' followed by digit
        {
            return Number();
            }
    
        // --- Handle Strings ---
        if (c == '"') {
            return string(); // string() handles consuming the rest
        }

        // --- Handle Identifiers & Other Keywords ---
        // This should only be reached if c wasn't part of an operator, number, or string start
        if (isalpha(c) || c == '_') { 
            std::string lexeme;
            lexeme += advance(); // Consume the first char (c)
            while (isalnum(peek()) || peek() == '_') {
                lexeme += advance();
            }
            auto it = keywords.find(lexeme);
            if (it != keywords.end()) {
                return {it->second, lexeme, startLine, startColumn};
                }
            return {TokenType::IDENTIFIER, lexeme, startLine, startColumn};
        }

        // --- Handle Remaining Single-Character Punctuation --- 
        // These should be characters not starting any multi-char token handled above
        // or prefixes of numbers/identifiers/strings
        switch (c) {
            case '+': advance(); return {TokenType::PLUS, "+", startLine, startColumn};
            // case '-': // Handled above
            case '*': advance(); return {TokenType::STAR, "*", startLine, startColumn};
            case '/': advance(); return {TokenType::SLASH, "/", startLine, startColumn};
            case '%': advance(); return {TokenType::MODULO, "%", startLine, startColumn};
            // case '<': // Handled above
            // case '>': // Handled above
            // case '=': // Handled above
            // case '!': // Handled above
            case '.': advance(); return {TokenType::DOT, ".", startLine, startColumn}; // Standalone DOT
            // case ':': // Handled above
            case ',': advance(); return {TokenType::COMMA, ",", startLine, startColumn};
            case ';': advance(); return {TokenType::SEMICOLON, ";", startLine, startColumn};
            case '(': advance(); return {TokenType::LEFT_PAREN, "(", startLine, startColumn};
            case ')': advance(); return {TokenType::RIGHT_PAREN, ")", startLine, startColumn};
            case '{': advance(); return {TokenType::LEFT_BRACE, "{", startLine, startColumn};
            case '}': advance(); return {TokenType::RIGHT_BRACE, "}", startLine, startColumn};
            case '[': advance(); return {TokenType::LEFT_BRACKET, "[", startLine, startColumn};
            case ']': advance(); return {TokenType::RIGHT_BRACKET, "]", startLine, startColumn};
        }
        
        // --- Error for Unknown Character --- 
        // Consume the unknown character before returning error
        advance(); 
        return {TokenType::ERROR, "Unexpected character: " + std::string(1, c), startLine, startColumn};
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
        int startLine = line;
        std::string lexeme = "";
        TokenType inferredType = TokenType::NUMBER; // Start assuming integer
        int startPos = current; // Initial position for loop detection
        
        // 1. Optional Leading Sign
        if (peek() == '-' || peek() == '+') {
            if (!isdigit(peekNext()) && peekNext() != '.') {
                // Let scanToken handle it as an operator
                // This should ideally not be reached if scanToken logic is correct
                return {TokenType::ERROR, "Sign not followed by digit or dot", startLine, startColumn};
            }
            lexeme += advance();
        }

        // Check for Hex/Binary/Octal prefix AFTER potential sign
        if (peek() == '0' && (peekNext() == 'x' || peekNext() == 'X' || 
                            peekNext() == 'b' || peekNext() == 'B' || 
                            peekNext() == 'o' || peekNext() == 'O')) {
            lexeme += advance(); // Consume '0'
            return handleSpecialNumber(lexeme, startColumn); // This handles hex floats too
        }

        // 2. Consume Integer Part (if any)
        bool hasLeadingDigits = isdigit(peek());
        if (hasLeadingDigits) {
            consumeDigits(lexeme);
        }

        // 3. Consume Decimal Part (if present)
        bool hasDecimalPoint = false;
        if (peek() == '.') {
            // Make sure it's not the '..' operator
            if (peekNext() == '.') {
                 // It's an integer followed by '..', return the integer part if any digits were read
                 if (hasLeadingDigits || !lexeme.empty()) { // Need digits before or sign 
                      return {TokenType::NUMBER, lexeme, startLine, startColumn};
                } else {
                      // Just a lone '.' followed by '.'? Let scanToken handle it.
                      return {TokenType::ERROR, "Invalid token start", startLine, startColumn};
                }
            }
            // Check if there are digits *after* the dot
            if (isdigit(peekNext())) {
                 hasDecimalPoint = true;
                 inferredType = TokenType::DOUBLE_LITERAL; // Now it's a float/double
                 lexeme += advance(); // Consume '.'
                 consumeDigits(lexeme); // Consume digits AFTER the dot
            } else if (hasLeadingDigits || !lexeme.empty()) {
                 // It's a number followed by a dot, but no digits after the dot (e.g., "123.")
                 // Still treat as a double literal in many languages
                 hasDecimalPoint = true;
                 inferredType = TokenType::DOUBLE_LITERAL;
                 lexeme += advance(); // Consume the dot
            } else {
                 // Just a lone dot, not followed by digits. Let scanToken handle it.
                 // If we started with a sign, it's an error here.
                 if (!lexeme.empty()) { // Had a sign
                       // Backtrack the sign
                       current--; column--;
                       char op = lexeme[0];
                       return { op == '+' ? TokenType::PLUS : TokenType::MINUS, std::string(1, op), startLine, startColumn };
                 }
                 // Otherwise, let scanToken handle the lone dot
                 return {TokenType::ERROR, "Invalid token start", startLine, startColumn}; 
            }
        }

        // Ensure we consumed *something* if we started with digits or a dot that formed part of a number
        if (!hasLeadingDigits && !hasDecimalPoint && lexeme.empty()) {
            // Didn't start with digit, didn't start with valid '.' + digit, didn't start with sign + digit/dot
            // This path indicates an error or logic flaw in scanToken calling Number()
            if (!isEnd()) advance(); // Prevent infinite loop
            return {TokenType::ERROR, "Number() called inappropriately", startLine, startColumn};
            }
            
        // 4. Consume Exponent (if present)
        bool hasExponent = false;
        if (peek() == 'e' || peek() == 'E') {
            char next = peekNext();
            if (isdigit(next) || ((next == '+' || next == '-') && isdigit(peekNextNext()))) {
                if (inferredType == TokenType::NUMBER) {
                    inferredType = TokenType::DOUBLE_LITERAL; // Int + exponent -> double
                }
                std::string beforeExp = lexeme;
                handleExponent(lexeme, false); // false = decimal exponent
                if (lexeme == beforeExp) { // handleExponent should have consumed digits
                    return {TokenType::ERROR, "Invalid exponent format (missing digits)", startLine, startColumn};
                    }
                hasExponent = true;
            }
            // else: 'e' not followed by digit/sign, treat as separate identifier
        }

        // 5. Consume Suffixes (f, F, l, L, u, U)
        char suffixPeek = peek();
        bool isFloatSuffix = (suffixPeek == 'f' || suffixPeek == 'F');
        bool isLongSuffix = (suffixPeek == 'l' || suffixPeek == 'L');
        bool isUnsignedSuffix = (suffixPeek == 'u' || suffixPeek == 'U');

        if (isFloatSuffix) {
            if (inferredType == TokenType::NUMBER) {
                advance(); // Consume f/F
                return {TokenType::ERROR, "Invalid suffix 'f'/'F' on integer literal", startLine, startColumn};
            }
            lexeme += advance(); // Consume suffix
            inferredType = TokenType::FLOAT_LITERAL;
            // Check for subsequent invalid suffixes
            if (peek() == 'l' || peek() == 'L' || peek() == 'u' || peek() == 'U') {
                advance();
                return {TokenType::ERROR, "Invalid suffix after 'f'/'F'", startLine, startColumn};
            }
        } else if (isLongSuffix || isUnsignedSuffix) {
            if (inferredType == TokenType::FLOAT_LITERAL || inferredType == TokenType::DOUBLE_LITERAL) {
                advance(); // Consume l/L/u/U
                return {TokenType::ERROR, "Invalid suffix (l/L/u/U) on floating-point literal", startLine, startColumn};
            }
            // Consume the suffix sequence robustly (simplified)
            std::string suffixCombo = "";
            bool firstL = false, secondL = false, firstU = false;
             while (true) {
                  char currentSuffix = peek();
                  if ((currentSuffix == 'l' || currentSuffix == 'L') && !secondL) {
                      if (!firstL) firstL = true;
                      else secondL = true;
                      suffixCombo += advance();
                  } else if ((currentSuffix == 'u' || currentSuffix == 'U') && !firstU) {
                      firstU = true;
                      suffixCombo += advance();
            } else {
                      break; 
                  }
             }
            lexeme += suffixCombo;
            inferredType = TokenType::NUMBER; // Still integer type conceptually
        }

        // Final check: If it had a decimal or exponent BUT no float suffix, it MUST be double
        if ((hasDecimalPoint || hasExponent) && inferredType == TokenType::NUMBER) {
            inferredType = TokenType::DOUBLE_LITERAL;
        }
        
        // Final check for progress to prevent infinite loops
        if (current == startPos) {
            if (!isEnd()) advance(); 
            return {TokenType::ERROR, "Number parsing failed to advance", startLine, startColumn};
        }

        return {inferredType, lexeme, startLine, startColumn};
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
        // lexeme already contains "0"
        int startLine = line;
        char prefix = advance(); // Consume x, b, or o
        lexeme += prefix;
        TokenType type = TokenType::NUMBER; // Default
        bool hasDecimalPoint = false;
        bool hasExponent = false;
        int startPos = current - 2; // Track from beginning of 0x/0b/0o
        
        if (prefix == 'x' || prefix == 'X') {
            // Hex Floats (e.g., 0x1.Ap+3) or Hex Ints (e.g., 0xFF)
            std::string beforeDigits = lexeme;
            consumeHexDigits(lexeme);
            bool hasMantissaDigits = (lexeme.size() > beforeDigits.size());
            
            if (peek() == '.') {
                // Check if digits followed the dot
                char nextAfterDot = peekNext();
                if (isxdigit(nextAfterDot)) {
                     hasDecimalPoint = true;
                     type = TokenType::DOUBLE_LITERAL;
                     lexeme += advance(); // consume dot
                     std::string beforeFraction = lexeme;
                consumeHexDigits(lexeme);
                     hasMantissaDigits = hasMantissaDigits || (lexeme.size() > beforeFraction.size());
                } else if (hasMantissaDigits) {
                     // Digits before dot, but not after (e.g., 0xFF.) - treat as double
                     hasDecimalPoint = true;
                     type = TokenType::DOUBLE_LITERAL;
                     lexeme += advance(); // consume dot
                } else {
                     // 0x. - Invalid
                     advance(); // Consume dot
                     return {TokenType::ERROR, "Invalid hex literal (0x.)", startLine, startColumn};
                }
            }

            if (!hasMantissaDigits) {
                 return {TokenType::ERROR, "Invalid hex literal (missing digits)", startLine, startColumn};
            }
            
            // Hex exponent (p/P)
            if (peek() == 'p' || peek() == 'P') {
                 char next = peekNext();
                 if (isdigit(next) || ((next == '+' || next == '-') && isdigit(peekNextNext()))) {
                      if (type == TokenType::NUMBER) type = TokenType::DOUBLE_LITERAL; // Becomes double
                      std::string beforeExp = lexeme;
                      handleExponent(lexeme, true); // true = hex float exponent
                      if (lexeme == beforeExp) {
                           return {TokenType::ERROR, "Invalid hex exponent format (missing digits)", startLine, startColumn};
                      }
                      hasExponent = true;
                 } else {
                      // p/P not followed by valid exponent - error or treat p as identifier?
                      // Let's error for now, as p is required for hex float exponent
                      return {TokenType::ERROR, "Invalid hex exponent (missing digits or sign)", startLine, startColumn};
            }
        }
            
            // Determine final type if not already float/double
            if (type == TokenType::NUMBER && (hasDecimalPoint || hasExponent)) {
                 type = TokenType::DOUBLE_LITERAL;
            }

            // Suffixes for hex literals
             char suffixPeek = peek();
             bool isFloatSuffix = (suffixPeek == 'f' || suffixPeek == 'F');
             bool isIntSuffix = (suffixPeek == 'l' || suffixPeek == 'L' || suffixPeek == 'u' || suffixPeek == 'U');

             if (isFloatSuffix) {
                 if (type == TokenType::NUMBER) {
                     advance();
                     return {TokenType::ERROR, "Invalid suffix 'f'/'F' on hex integer literal", startLine, startColumn};
                 }
                 lexeme += advance();
                 type = TokenType::FLOAT_LITERAL;
                  if (peek() == 'l' || peek() == 'L' || peek() == 'u' || peek() == 'U') {
                      advance();
                     return {TokenType::ERROR, "Invalid suffix after 'f'/'F'", startLine, startColumn};
                 }
             } else if (isIntSuffix) {
                  if (type == TokenType::FLOAT_LITERAL || type == TokenType::DOUBLE_LITERAL) {
                       advance();
                       return {TokenType::ERROR, "Invalid suffix (l/L/u/U) on hex float literal", startLine, startColumn};
                  }
                  // Consume integer suffixes robustly (simplified)
                  std::string suffixCombo = "";
                  // ... (Add robust suffix consumption similar to Number()) ...
                  while(isalnum(peek())) { // Simplistic consumption - needs refinement for order (U before L)
                       char s = peek();
                       if (s == 'l' || s == 'L' || s == 'u' || s == 'U') {
                            suffixCombo += advance(); 
                       } else break;
                  }
                  lexeme += suffixCombo;
                  type = TokenType::NUMBER; 
             }
             
             if (current == startPos) { // Check progress
                 if (!isEnd()) advance();
                  return {TokenType::ERROR, "Hex number parsing failed to advance", startLine, startColumn};
            }
             return {type, lexeme, startLine, startColumn};

        } else if (prefix == 'b' || prefix == 'B') {
            // Binary - Must be integer
            if (!consumeBinaryDigits(lexeme)) { 
                 return {TokenType::ERROR, "Invalid binary literal (missing digits)", startLine, startColumn};
            }
            type = TokenType::NUMBER; 
        } else if (prefix == 'o' || prefix == 'O') {
            // Octal - Must be integer
             if (!consumeOctalDigits(lexeme)) { 
                 return {TokenType::ERROR, "Invalid octal literal (missing digits)", startLine, startColumn};
             }
            type = TokenType::NUMBER; 
        } else {
             return {TokenType::ERROR, "Invalid prefix after '0'", startLine, startColumn};
        }
        
        // Suffixes for binary/octal (integers only)
        char suffixPeek = peek();
        if (suffixPeek == 'f' || suffixPeek == 'F') {
             advance();
             return {TokenType::ERROR, "Invalid suffix 'f'/'F' on binary/octal literal", startLine, startColumn};
        } else if (suffixPeek == 'l' || suffixPeek == 'L' || suffixPeek == 'u' || suffixPeek == 'U'){
             // Consume integer suffixes robustly (simplified)
              std::string suffixCombo = "";
              // ... (Add robust suffix consumption similar to Number()) ...
              while(isalnum(peek())) { // Simplistic consumption
                   char s = peek();
                   if (s == 'l' || s == 'L' || s == 'u' || s == 'U') {
                       suffixCombo += advance();
                   } else break;
              }
             lexeme += suffixCombo;
        }
        
        if (current == startPos) { // Check progress
             if (!isEnd()) advance();
             return {TokenType::ERROR, "Special number parsing failed to advance", startLine, startColumn};
        }

        return {type, lexeme, startLine, startColumn};
    }
    

    
    
    Token Lexer::string() {
        int startColumn = column; // Column where the opening " was
        int startLine = line;
        std::string lexeme;
        advance(); // Consume the opening " that scanToken detected
        
        // Read until the closing double quote
        while (!isEnd() && peek() != '"') {
            char c = peek();
            
            // Handle escape sequences
            if (c == '\\' && !isEnd()) {
                advance(); // Consume the backslash
                if (isEnd()) {
                    // Error: Unterminated escape sequence at end of file
                    return {TokenType::ERROR, "Unterminated escape sequence at EOF", startLine, startColumn};
                }
                char escapedChar = advance(); // Consume the character after backslash
                switch (escapedChar) {
                    case 'n': lexeme += '\n'; break;
                    case 't': lexeme += '\t'; break;
                    case 'r': lexeme += '\r'; break;
                    case '\\': lexeme += '\\'; break;
                    case '\"': lexeme += '\"'; break;
                    // Add other escapes if needed (e.g., '\0')
                    default:
                        // Optional: Treat unknown escapes as literal backslash + char
                        // lexeme += '\\';
                        // lexeme += escapedChar;
                        // OR: Report an error for unknown escape sequences
                        return {TokenType::ERROR, "Unknown escape sequence: \\" + std::string(1, escapedChar), line, column - 2};
                }
            } else if (c == '\n') {
                 // Error: Newline inside string literal without escaping
                 advance(); // Consume the newline to report error position correctly
                 return {TokenType::ERROR, "Unterminated string literal (newline encountered)", startLine, startColumn};
            } else {
                // Regular character
                lexeme += advance(); // Consume the character
            }
        }
        
        // Check for unterminated string
        if (isEnd()) {
            return {TokenType::ERROR, "Unterminated string literal", startLine, startColumn};
        }
        
        // Consume the closing double quote
        advance();
        
        // Return the STRING token with the processed lexeme (escape sequences interpreted)
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
    