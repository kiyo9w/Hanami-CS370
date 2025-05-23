#include <iostream>
#include <vector>
#include <string>
#include <fstream>  // For file streams
#include <sstream>  // For reading file content
#include "../common/token.h"
#include "lexer.h" // Include Lexer class definition
#include "../common/utils.h" // Include the header for tokenTypeToString declaration
#include <chrono>

// Helper function to escape strings for output file
std::string escapeStringForOutput(const std::string& s) {
    std::string escaped;
    escaped.reserve(s.length()); // Pre-allocate for potential expansion
    for (char c : s) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '\"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            // Add other escapes if needed
            default: escaped += c; break;
        }
    }
    return escaped;
}

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
        std::cout << " (\\\""; // Keep std::quoted for console debug printing if desired
        std::cout << escapeStringForOutput(token.lexeme); // Use escaping for console too? Or keep original lexeme? Let's escape for consistency.
         std::cout << "\\\")";
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

    //start_time
    auto start_time = std::chrono::steady_clock::now();

    Lexer lexer(sourceCode);
    std::vector<Token> tokens;
    bool lexSuccessful = true;

    try {
        tokens = lexer.scanTokens(); // Get all tokens
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred during lexing: " << e.what() << std::endl;
        // Get the time point just after execution
        auto end_time = std::chrono::steady_clock::now();

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

             // Get the time point just after execution
            auto end_time = std::chrono::steady_clock::now();
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

        // Get the time point just after execution
        auto end_time = std::chrono::steady_clock::now();

        return 1;
    }

    // Write tokens to file in the specified format
    for (const auto& token : tokens) {
        // Use the shared tokenTypeToString function
        std::string typeStr = tokenTypeToString(token.type);
        outFile << typeStr;
        
        // Add lexeme if applicable, escaping strings/paths
        if (token.type == TokenType::IDENTIFIER ||
            token.type == TokenType::NUMBER ||
            token.type == TokenType::FLOAT_LITERAL || // Added
            token.type == TokenType::DOUBLE_LITERAL || // Added
            token.type == TokenType::STRING ||
            token.type == TokenType::STYLE_INCLUDE ||
            token.type == TokenType::ERROR) // Include lexeme for ERROR tokens too
        {
             outFile << " ";
             // Manually escape the lexeme before writing
             outFile << escapeStringForOutput(token.lexeme); 
        }
        
        outFile << " " << token.line << " " << token.column << std::endl;

        // Stop writing after EOF_TOKEN
        if (token.type == TokenType::EOF_TOKEN) {
            break; 
        }
    }

    // Get the time point just after execution
    auto end_time = std::chrono::steady_clock::now();

    //get the time
    auto duration = end_time - start_time;
    // Correct way to print the duration in milliseconds:
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    outFile.close();
    std::cout << "Tokens successfully written to " << outputFilename << std::endl;

    std::cout<<"Time execution: "<< duration_ms.count() <<" ms"<<'\n';
    return 0;
}
