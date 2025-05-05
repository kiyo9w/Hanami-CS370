#include <iostream>
#include <vector>
#include <string>
#include <fstream>  // For file streams
#include <sstream>  // For reading file content
#include <iomanip>  // For std::quoted
#include "../common/token.h"
#include "lexer.h" // Include Lexer class definition
#include "../common/utils.h" // Include the header for tokenTypeToString declaration

// Print token information in a readable format
void printToken(const Token& token) {
    // Use the shared tokenTypeToString from utils.h
    std::string typeStr = tokenTypeToString(token.type);

    std::cout << "Token: " << typeStr;

    // Print lexeme for identifiers, numbers, literals, and strings
    if (token.type == TokenType::IDENTIFIER ||
        token.type == TokenType::NUMBER ||
        token.type == TokenType::FLOAT_LITERAL || // Added
        token.type == TokenType::DOUBLE_LITERAL || // Added
        token.type == TokenType::STRING ||
        token.type == TokenType::STYLE_INCLUDE ||
        token.type == TokenType::ERROR) {
        std::cout << " (\"";
        if (token.type == TokenType::STRING || token.type == TokenType::STYLE_INCLUDE) {
            std::cout << std::quoted(token.lexeme);
        } else {
            std::cout << token.lexeme;
        }
         std::cout << "\")";
    }

    std::cout << " at line " << token.line << ", column " << token.column << std::endl;
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
        // Use the shared tokenTypeToString function
        std::string typeStr = tokenTypeToString(token.type);
        outFile << typeStr;
        
        // Add lexeme if applicable, handling quotes for strings/paths
        if (token.type == TokenType::IDENTIFIER ||
            token.type == TokenType::NUMBER ||
            token.type == TokenType::FLOAT_LITERAL || // Added
            token.type == TokenType::DOUBLE_LITERAL || // Added
            token.type == TokenType::STRING ||
            token.type == TokenType::STYLE_INCLUDE ||
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
