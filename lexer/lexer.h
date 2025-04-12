#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>


enum class TokenType {
    // Hanami Keywords
    GARDEN,     // namespace
    SPECIES,    // class
    OPEN,       // public
    HIDDEN,     // private
    GUARDED,    // protected
    GROW,       // function declaration
    BLOSSOM,    // return
    STYLE,      // #include
    BLOOM,      // std::cout
    WATER,      // std::cin
    BRANCH,     // if/else if
    ELSE,       // else

    // Control Flow (keeping common ones)
    WHILE, FOR,

    // Literals
    IDENTIFIER, NUMBER, STRING, TRUE, FALSE,

    // Operators
    PLUS, MINUS, STAR, SLASH, // Đổi MULTIPLY thành STAR, DIVIDE thành SLASH để nhất quán
    ASSIGN,                   // Toán tử gán (=)
    EQUAL,                    // Toán tử so sánh (==)
    NOT_EQUAL,                // Toán tử so sánh (!=)
    LESS, LESS_EQUAL,         // Thêm LESS_EQUAL (<=)
    GREATER, GREATER_EQUAL,   // Thêm GREATER_EQUAL (>=)
    AND, OR, NOT,
    MODULO,                   // Thêm toán tử %
    //OPERATOR,                  // Operator chung, xu li cu the thi de thang sau

    // Hanami Specific Operators
    STREAM_OUT,  // << (for bloom)
    STREAM_IN,   // >> (for water)
    ARROW,       // -> (for function return type)

    // Punctuation
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET, COMMA, SEMICOLON, DOT,
    COLON,      // Dùng cho open:, hidden:, guarded:

    // Special
    COMMENT, EOF_TOKEN, ERROR,
    
    //More
    STYLE_INCLUDE, SCOPE_RESOLUTION
};




struct Token
{
    TokenType type;
    std::string lexeme; //string duoc phan loai roi
    int line;   //
    int column;
};


class Lexer{
    private:
        std::string source;
        std::vector<Token> tokens;
        size_t current = 0;
        int line = 1;
        int column = 1;

        //hashmap for the keyword
        std::unordered_map<std::string, TokenType> keywords;
        
        //methods
        char advance();     // read next char
        char peek();        // see the char without goto it
        char peekNext();    // see the next char from current location without goto it
        char peekNextNext();
        bool isEnd();        // check if source is end or not
        bool match(char expected);       //check and read if match


    public:
    Lexer(const std::string& source) : source(source), current(0), line(1), column(1) {
        // Khởi tạo bảng từ khóa
        initKeywords();
    }
        std::vector<Token> scanTokens();
        Token scanToken();      // scan ONE token
        void initKeywords();

    //methods
        Token identifier();     // process identifier and keywords
        Token Number();         //process number
        Token string();         //process string
        Token skipComment();       // skip Comment;
        Token handleSpecialNumber(std::string& lexeme, int startColumn);

        void consumeDigits(std::string& lexeme);
        void consumeHexDigits(std::string& lexeme);
        bool consumeBinaryDigits(std::string& lexeme);
        bool consumeOctalDigits(std::string& lexeme);
        void handleExponent(std::string& lexeme, bool isHexFloat);
        void handleTypeSuffix(std::string& lexeme);

        //hmm
        void skipWhitespace();

};
