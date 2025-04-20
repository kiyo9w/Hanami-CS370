#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <map>
#include <iomanip> // For json pretty printing
#include <regex>


#include "parser.h"
#include "../common/token.h" // Includes Token struct and TokenType enum
#include "../common/json.hpp" // For JSON serialization
#include "../common/utils.h" // Include for stringToTokenType

// Helper function to convert string representation of TokenType back to enum
// NOTE: This needs to be kept in sync with the TokenType enum in token.h
//       and the strings used by the lexer's output.
/*
std::map<std::string, TokenType> tokenTypeMap = {
    {"GARDEN", TokenType::GARDEN}, {"SPECIES", TokenType::SPECIES}, {"OPEN", TokenType::OPEN}, 
    {"HIDDEN", TokenType::HIDDEN}, {"GUARDED", TokenType::GUARDED}, {"GROW", TokenType::GROW}, 
    {"BLOSSOM", TokenType::BLOSSOM}, {"STYLE", TokenType::STYLE}, {"BLOOM", TokenType::BLOOM}, 
    {"WATER", TokenType::WATER}, {"BRANCH", TokenType::BRANCH}, {"ELSE", TokenType::ELSE}, 
    {"WHILE", TokenType::WHILE}, {"FOR", TokenType::FOR}, {"IDENTIFIER", TokenType::IDENTIFIER},
    {"NUMBER", TokenType::NUMBER}, {"STRING", TokenType::STRING}, {"TRUE", TokenType::TRUE}, 
    {"FALSE", TokenType::FALSE}, {"PLUS", TokenType::PLUS}, {"MINUS", TokenType::MINUS}, 
    {"STAR", TokenType::STAR}, {"SLASH", TokenType::SLASH}, {"ASSIGN", TokenType::ASSIGN}, 
    {"EQUAL", TokenType::EQUAL}, {"NOT_EQUAL", TokenType::NOT_EQUAL}, {"LESS", TokenType::LESS}, 
    {"LESS_EQUAL", TokenType::LESS_EQUAL}, {"GREATER", TokenType::GREATER}, {"GREATER_EQUAL", TokenType::GREATER_EQUAL},
    {"AND", TokenType::AND}, {"OR", TokenType::OR}, {"NOT", TokenType::NOT}, 
    {"MODULO", TokenType::MODULO}, {"STREAM_OUT", TokenType::STREAM_OUT}, {"STREAM_IN", TokenType::STREAM_IN}, 
    {"ARROW", TokenType::ARROW}, {"LEFT_PAREN", TokenType::LEFT_PAREN}, {"RIGHT_PAREN", TokenType::RIGHT_PAREN}, 
    {"LEFT_BRACE", TokenType::LEFT_BRACE}, {"RIGHT_BRACE", TokenType::RIGHT_BRACE}, {"LEFT_BRACKET", TokenType::LEFT_BRACKET}, 
    {"RIGHT_BRACKET", TokenType::RIGHT_BRACKET}, {"COMMA", TokenType::COMMA}, {"SEMICOLON", TokenType::SEMICOLON},
    {"DOT", TokenType::DOT}, {"COLON", TokenType::COLON}, {"COMMENT", TokenType::COMMENT}, // Comment might not appear in output
    {"EOF_TOKEN", TokenType::EOF_TOKEN}, {"ERROR", TokenType::ERROR}, {"STYLE_INCLUDE", TokenType::STYLE_INCLUDE}, 
    {"SCOPE_RESOLUTION", TokenType::SCOPE_RESOLUTION}
};

TokenType stringToTokenType(const std::string& typeStr) {
    auto it = tokenTypeMap.find(typeStr);
    if (it != tokenTypeMap.end()) {
        return it->second;
    } else {
        // Handle unknown token type string - maybe return ERROR or throw
        std::cerr << "Warning: Unknown token type string encountered: " << typeStr << std::endl;
        return TokenType::ERROR; // Or throw an exception
    }
}
*/

// Function to read tokens from the specified file format
// Function to read tokens from the specified file format
std::vector<Token> readTokensFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        throw std::runtime_error("Error: Could not open input token file: " + filename);
    }

    std::vector<Token> tokens;
    std::string line;
    int currentLineNum = 0;

    while (std::getline(inFile, line)) {
        currentLineNum++;
        std::stringstream ss(line);
        std::string tokenTypeStr;
        std::string lexeme = "";
        int tokenLine = 0;
        int tokenColumn = 0;

        // Đọc loại token (như "STYLE", "IDENTIFIER", "STRING", v.v.)
        ss >> tokenTypeStr;
        if (ss.fail()) {
            std::cerr << "Warning: Failed to read token type on line " << currentLineNum << " of " << filename << std::endl;
            continue;
        }

        TokenType type = stringToTokenType(tokenTypeStr);

        // Đọc lexeme nếu token là loại cần lexeme
        if (type == TokenType::IDENTIFIER || type == TokenType::NUMBER || 
            type == TokenType::STRING || type == TokenType::STYLE_INCLUDE) {
            // Đối với STRING và STYLE_INCLUDE, lexeme có thể được bao quanh bởi dấu ngoặc kép
            char nextChar = ss.peek();
            while(isspace(nextChar)) {
                ss.ignore();
                nextChar = ss.peek();
            }
            
            if (type == TokenType::STRING || type == TokenType::STYLE_INCLUDE) {
                if (nextChar == '"') {
                    ss >> std::quoted(lexeme); // Đọc chuỗi trong dấu ngoặc kép
                } else {
                    ss >> lexeme; // Đọc bình thường nếu không có dấu ngoặc kép
                }
            } else {
                ss >> lexeme; // Đọc lexeme thông thường cho IDENTIFIER hoặc NUMBER
            }
            
            if (ss.fail()) {
                std::cerr << "Warning: Failed to read lexeme for " << tokenTypeStr << " on line " << currentLineNum << std::endl;
                continue;
            }
        }

        // Đọc số dòng và số cột
        ss >> tokenLine >> tokenColumn;
        if (ss.fail()) {
            std::cerr << "Warning: Failed to read line/column numbers for token on line " << currentLineNum << " of " << filename << std::endl;
            continue;
        }

        tokens.push_back({type, lexeme, tokenLine, tokenColumn});
    }

    return tokens;
}




int main(int argc, char* argv[]) {
    std::string inputFilename = "input/input.tokens"; // Default input file
    std::string outputFilename = "output/output.ast"; // Default output file

    if (argc > 1) {
        inputFilename = std::string(argv[1]);
    }
    if (argc > 2) {
        outputFilename = std::string(argv[2]);
    }

    std::cout << "Parser Module" << std::endl;
    std::cout << "Reading tokens from: " << inputFilename << std::endl;

    std::vector<Token> tokens;
    try {
        tokens = readTokensFromFile(inputFilename);
        
        // Optional: Print tokens read for verification
        // std::cout << "--- Tokens Read ---" << std::endl;
        // for (const auto& token : tokens) {
        //     std::cout << static_cast<int>(token.type) << " (" << token.lexeme << ") L" << token.line << " C" << token.column << std::endl;
        // }
        // std::cout << "-------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error reading tokens: " << e.what() << std::endl;
        return 1;
    }

    if (tokens.empty()) {
        std::cerr << "Error: No tokens were read from the input file." << std::endl;
        return 1;
    }

    // Add EOF if missing (parser expects it)
     if (tokens.back().type != TokenType::EOF_TOKEN) {
         std::cerr << "Warning: Adding missing EOF_TOKEN to token stream." << std::endl;
         int lastLine = tokens.empty() ? 1 : tokens.back().line;
         int lastCol = tokens.empty() ? 1 : tokens.back().column + 1;
         tokens.push_back({TokenType::EOF_TOKEN, "", lastLine, lastCol});
     }

    std::cout << "Parsing token stream..." << std::endl;
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> astRoot = nullptr;
    bool parseSuccessful = true;

    try {
        astRoot = parser.parse();
    } catch (const ParseError& e) {
        // Parser::error already printed details
        std::cerr << "Parsing failed due to syntax errors." << std::endl;
        parseSuccessful = false;
        // Continue to allow partial output if desired, or return here
         return 1; // Exit after first parse error
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred during parsing: " << e.what() << std::endl;
        return 1;
    }

    if (!astRoot) {
        std::cerr << "Error: Parsing resulted in a null AST root." << std::endl;
        return 1;
    }
    
    if(parseSuccessful){
        std::cout << "Parsing completed successfully." << std::endl;
        std::cout << "Writing AST to: " << outputFilename << std::endl;
    
        // Serialize AST to JSON
        nlohmann::json astJson = astRoot->toJson();
    
        // Write JSON to output file
        std::ofstream outFile(outputFilename);
        if (!outFile) {
            std::cerr << "Error: Could not open output AST file: " << outputFilename << std::endl;
            return 1;
        }
    
        // Write pretty-printed JSON
        outFile << std::setw(4) << astJson << std::endl;
        outFile.close();
    
        std::cout << "AST successfully written to " << outputFilename << std::endl;
    } else {
         std::cout << "Parsing finished with errors. AST was not generated." << std::endl;
         return 1; // Indicate failure
    }


    return 0;
}