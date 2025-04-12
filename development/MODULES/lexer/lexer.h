#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

// Include the definitions from the common header
#include "../common/token.h"

// Only declare the Lexer class here
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
    Lexer(const std::string& source);
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
