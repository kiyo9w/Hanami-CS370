#include <iostream>
#include <vector>
#include <string>
#include <fstream>  // For file streams
#include <sstream>  // For reading file content
#include <iomanip>  // For std::quoted
#include "../common/token.h"
#include "lexer.h" // Include Lexer class definition
#include <map>

// Print token information in a readable format
void printToken(const Token& token) {
    // Map of token types to string representations
    static const char* tokenTypeStrings[] = {
        "GARDEN", "SPECIES", "OPEN", "HIDDEN", "GUARDED", 
        "GROW", "BLOSSOM", "STYLE", "BLOOM", "WATER",
        "BRANCH", "ELSE", "WHILE", "FOR", "IDENTIFIER",
        "NUMBER", "STRING", "TRUE", "FALSE", "PLUS",
        "MINUS", "STAR", "SLASH", "ASSIGN", "EQUAL",
        "NOT_EQUAL", "LESS", "LESS_EQUAL", "GREATER", "GREATER_EQUAL",
        "AND", "OR", "NOT", "MODULO", "STREAM_OUT",
        "STREAM_IN", "ARROW", "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE",
        "RIGHT_BRACE", "LEFT_BRACKET", "RIGHT_BRACKET", "COMMA", "SEMICOLON",
        "DOT", "COLON", "COMMENT", "EOF_TOKEN", "ERROR", "STYLE_INCLUDE", "SCOPE_RESOLUTION"
    };
    
    // Get index based on token type (safely)
    int index = static_cast<int>(token.type);
    const char* typeStr = (index >= 0 && index < sizeof(tokenTypeStrings)/sizeof(tokenTypeStrings[0])) 
                         ? tokenTypeStrings[index] : "UNKNOWN";
    
    std::cout << "Token: " << typeStr;
    
    // Print lexeme for identifiers, numbers, and strings
    if (token.type == TokenType::IDENTIFIER || 
        token.type == TokenType::NUMBER || 
        token.type == TokenType::STRING) {
        std::cout << " (\"" << token.lexeme << "\")";
    }
    
    std::cout << " at line " << token.line << ", column " << token.column << std::endl;
}

// Function to convert TokenType enum to string (reverse of parser's map)
// Keep this in sync with common/token.h
std::string tokenTypeToString(TokenType type) {
    // This map should contain all entries from the TokenType enum
     static const std::map<TokenType, std::string> typeToStringMap = {
        {TokenType::GARDEN, "GARDEN"}, {TokenType::SPECIES, "SPECIES"}, {TokenType::OPEN, "OPEN"}, 
        {TokenType::HIDDEN, "HIDDEN"}, {TokenType::GUARDED, "GUARDED"}, {TokenType::GROW, "GROW"}, 
        {TokenType::BLOSSOM, "BLOSSOM"}, {TokenType::STYLE, "STYLE"}, {TokenType::BLOOM, "BLOOM"}, 
        {TokenType::WATER, "WATER"}, {TokenType::BRANCH, "BRANCH"}, {TokenType::ELSE, "ELSE"}, 
        {TokenType::WHILE, "WHILE"}, {TokenType::FOR, "FOR"}, {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::NUMBER, "NUMBER"}, {TokenType::STRING, "STRING"}, {TokenType::TRUE, "TRUE"}, 
        {TokenType::FALSE, "FALSE"}, {TokenType::PLUS, "PLUS"}, {TokenType::MINUS, "MINUS"}, 
        {TokenType::STAR, "STAR"}, {TokenType::SLASH, "SLASH"}, {TokenType::ASSIGN, "ASSIGN"}, 
        {TokenType::EQUAL, "EQUAL"}, {TokenType::NOT_EQUAL, "NOT_EQUAL"}, {TokenType::LESS, "LESS"}, 
        {TokenType::LESS_EQUAL, "LESS_EQUAL"}, {TokenType::GREATER, "GREATER"}, {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::AND, "AND"}, {TokenType::OR, "OR"}, {TokenType::NOT, "NOT"}, 
        {TokenType::MODULO, "MODULO"}, {TokenType::STREAM_OUT, "STREAM_OUT"}, {TokenType::STREAM_IN, "STREAM_IN"}, 
        {TokenType::ARROW, "ARROW"}, {TokenType::LEFT_PAREN, "LEFT_PAREN"}, {TokenType::RIGHT_PAREN, "RIGHT_PAREN"}, 
        {TokenType::LEFT_BRACE, "LEFT_BRACE"}, {TokenType::RIGHT_BRACE, "RIGHT_BRACE"}, {TokenType::LEFT_BRACKET, "LEFT_BRACKET"}, 
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"}, {TokenType::COMMA, "COMMA"}, {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::DOT, "DOT"}, {TokenType::COLON, "COLON"}, {TokenType::COMMENT, "COMMENT"},
        {TokenType::EOF_TOKEN, "EOF_TOKEN"}, {TokenType::ERROR, "ERROR"}, {TokenType::STYLE_INCLUDE, "STYLE_INCLUDE"}, 
        {TokenType::SCOPE_RESOLUTION, "SCOPE_RESOLUTION"}
    };
    auto it = typeToStringMap.find(type);
    if (it != typeToStringMap.end()) {
        return it->second;
    } else {
        return "UNKNOWN";
    }
}

int main(int argc, char* argv[]) { 
    std::string inputFilename = "input/input.hanami"; // Default input source file
    std::string outputFilename = "output/output.tokens"; // Default output token file

    if (argc > 1) {
        inputFilename = std::string(argv[1]);
    }
    // Optional: Allow specifying output file via command line
    // if (argc > 2) {
    //     outputFilename = std::string(argv[2]);
    // }

    std::cout << "Lexer Module" << std::endl;
    std::cout << "Reading source from: " << inputFilename << std::endl;

    std::ifstream inFile(inputFilename);
    if (!inFile) {
        std::cerr << "Error: Could not open input file: " << inputFilename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string sourceCode = buffer.str();
    inFile.close();

    if (sourceCode.empty()) {
        std::cout << "Input file is empty. Nothing to tokenize." << std::endl;
        // Create an empty output file or one with just EOF?
         std::ofstream outFile(outputFilename);
         if (!outFile) {
              std::cerr << "Error: Could not open output file: " << outputFilename << std::endl;
              return 1;
         }
         // Write just EOF for consistency
         outFile << tokenTypeToString(TokenType::EOF_TOKEN) << " 1 1" << std::endl; 
         outFile.close();
         std::cout << "Empty token file written to: " << outputFilename << std::endl;
        return 0;
    }

    std::cout << "Tokenizing source code..." << std::endl;
    Lexer lexer(sourceCode);
    std::vector<Token> tokens;
    bool lexSuccessful = true;

    try {
        tokens = lexer.scanTokens(); // Get all tokens
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred during lexing: " << e.what() << std::endl;
        lexSuccessful = false;
        return 1;
    }

    // Check for errors reported during lexing (ERROR tokens)
    for(const auto& token : tokens) {
        if (token.type == TokenType::ERROR) {
            std::cerr << "Lexing error encountered: " << token.lexeme 
                      << " at line " << token.line << ", column " << token.column << std::endl;
            lexSuccessful = false;
            // Decide whether to stop or continue after first error
             // return 1; // Stop on first error
        }
    }

    if (!lexSuccessful) {
        std::cerr << "Lexing failed due to errors. Token file not generated." << std::endl;
        return 1;
    }

    std::cout << "Lexing completed successfully." << std::endl;
    std::cout << "Writing tokens to: " << outputFilename << std::endl;

    std::ofstream outFile(outputFilename);
    if (!outFile) {
        std::cerr << "Error: Could not open output file: " << outputFilename << std::endl;
        return 1;
    }

    // Write tokens to file in the specified format
    for (const auto& token : tokens) {
        std::string typeStr = tokenTypeToString(token.type);
        outFile << typeStr;
        
        // Add lexeme if applicable, handling quotes for strings/paths
        if (token.type == TokenType::IDENTIFIER || token.type == TokenType::NUMBER || 
            token.type == TokenType::STRING || token.type == TokenType::STYLE_INCLUDE ||
            token.type == TokenType::ERROR) // Include lexeme for ERROR tokens too
        {
             outFile << " ";
             if (token.type == TokenType::STRING || token.type == TokenType::STYLE_INCLUDE) {
                 outFile << std::quoted(token.lexeme); // Add quotes for strings/paths
             } else {
                 outFile << token.lexeme;
             }
        }
        
        outFile << " " << token.line << " " << token.column << std::endl;

        // Stop writing after EOF_TOKEN
        if (token.type == TokenType::EOF_TOKEN) {
            break; 
        }
    }

    outFile.close();
    std::cout << "Tokens successfully written to " << outputFilename << std::endl;

    return 0;
}
