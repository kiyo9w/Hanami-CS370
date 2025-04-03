#pragma once

#include <string>
#include <vector>
#include <iostream>


enum class TokenType{
    //keyword
    IF, ELSE, WHILE, FOR, RETURN, FUNCTION,

    //Literals
    IDENTIFIER, NUMBER, STRING, TRUE, FALSE,

    //Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN, EQUAL, NOT_EQUAL,
    LESS, GREATER, AND, OR, NOT,

    //Punctuation
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET, COMMA, SEMICOLON, DOT,

    //Special
    COMMENT, EOF_TOKEN, ERROR

    //Others add later. fking trung, hard vl
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

        //methods
        char advance();     // read next char
        char peek();        // see the char without goto it
        char peekNext();    // see the next char from current location without goto it
        bool isEnd();        // check if source is end or not
        bool match(char expected);       //check and read if match


    public:
        Lexer(const std::string& source) : source(source) {};
        std::vector<Token> scanTokens();
        Token scanToken();      // scan ONE token

    //methods
        Token identifier();     // process identifier and keywords
        Token Number();         //process number
        Token string();         //process string
        Token skipComment();       // skip Comment;
};
