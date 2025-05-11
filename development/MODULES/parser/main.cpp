#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <map>
#include <iomanip> // For json pretty printing
#include <regex>
#include <chrono>

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

// Function to read tokens from the specified file format (simplified)
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
        line.erase(0, line.find_first_not_of(" \t\n\r")); // Trim leading
        line.erase(line.find_last_not_of(" \t\n\r") + 1); // Trim trailing
        if (line.empty()) {
            continue;
        }

        std::string tokenTypeStr;
        std::string lexemeStr = "";
        int tokenLine = 0;
        int tokenColumn = 0;

        // 1. Find the last space (must exist before column number)
        size_t lastSpacePos = line.find_last_of(" \t");
        if (lastSpacePos == std::string::npos) {
            std::cerr << "Warning [L" << currentLineNum << "]: Malformed token line (no space before line/col?): '" << line << "'" << std::endl;
            continue;
        }

        // 2. Extract Column Number
        try {
            tokenColumn = std::stoi(line.substr(lastSpacePos + 1));
        } catch (...) {
            std::cerr << "Warning [L" << currentLineNum << "]: Malformed token line (invalid column number?): '" << line << "'" << std::endl;
            continue;
        }

        // 3. Find the second-to-last space (must exist before line number)
        size_t secondLastSpacePos = line.find_last_of(" \t", lastSpacePos - 1);
        if (secondLastSpacePos == std::string::npos) {
            std::cerr << "Warning [L" << currentLineNum << "]: Malformed token line (no space before line number?): '" << line << "'" << std::endl;
                continue;
            }

        // 4. Extract Line Number
        try {
            tokenLine = std::stoi(line.substr(secondLastSpacePos + 1, lastSpacePos - (secondLastSpacePos + 1)));
        } catch (...) {
            std::cerr << "Warning [L" << currentLineNum << "]: Malformed token line (invalid line number?): '" << line << "'" << std::endl;
            continue;
        }

        // 5. Extract Token Type and Potential Lexeme
        TokenType currentType; // Declare outside if/else
        size_t firstSpacePos = line.find_first_of(" \\t");
        if (firstSpacePos == std::string::npos || firstSpacePos >= secondLastSpacePos) {
            // No space between type and line number -> No lexeme
            tokenTypeStr = line.substr(0, secondLastSpacePos + 1);
            tokenTypeStr.erase(tokenTypeStr.find_last_not_of(" \\t") + 1); // Trim trailing spaces
            lexemeStr = "";
            currentType = stringToTokenType(tokenTypeStr); // Assign type here too
        } else {
            // Space found -> Assume Type is before first space, Lexeme is between first and second-to-last space
            tokenTypeStr = line.substr(0, firstSpacePos);
            lexemeStr = line.substr(firstSpacePos + 1, secondLastSpacePos - firstSpacePos); // Original length calculation
            
            // Trim BOTH leading and trailing whitespace unconditionally first
            while (!lexemeStr.empty() && std::isspace(lexemeStr.front())) {
                lexemeStr.erase(0, 1); // Remove leading space
            }
            while (!lexemeStr.empty() && std::isspace(lexemeStr.back())) {
                lexemeStr.pop_back(); // Remove trailing space
            }
            
            // Convert string to TokenType enum
            currentType = stringToTokenType(tokenTypeStr);
            
            // Unescape ONLY if it's a string literal
            if (currentType == TokenType::STRING) {
                 std::string temp_lexeme;
                 temp_lexeme.reserve(lexemeStr.length());
                 for(size_t i = 0; i < lexemeStr.length(); ++i){
                     if(lexemeStr[i] == '\\' && i + 1 < lexemeStr.length()){
                         char nextChar = lexemeStr[++i]; // Consume slash, get next char
                         switch (nextChar) {
                             case '\"': temp_lexeme += '\"'; break;
                             case '\\': temp_lexeme += '\\'; break;
                             case 'n': temp_lexeme += '\n'; break;
                             case 'r': temp_lexeme += '\r'; break;
                             case 't': temp_lexeme += '\t'; break;
                             default: // Not a known escape, keep backslash and char
                                 temp_lexeme += '\\';
                                 temp_lexeme += nextChar;
                                 break;
                         }
                     } else {
                         temp_lexeme += lexemeStr[i];
                     }
                 }
                 lexemeStr = temp_lexeme;
            }
        }

        // 6. Add Token (Use already converted type)
        tokens.push_back({currentType, lexemeStr, tokenLine, tokenColumn});
    }

    inFile.close();
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

    //start_time
    auto start_time = std::chrono::steady_clock::now();
     
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> astRoot = nullptr;
    bool parseErrorOccurred = false; // Flag to track if any ParseError was caught

    try {
        astRoot = parser.parse();
    } catch (const ParseError& e) {
        // Parser::error already printed details
        std::cerr << "Parsing failed due to syntax errors." << std::endl;

        //end time
        auto end_time = std::chrono::steady_clock::now();

        parseErrorOccurred = true; // Set the flag
        // Continue to allow partial output if desired, or return here
         // return 1; // Exit after first parse error - Keep commented to allow seeing multiple errors
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred during parsing: " << e.what() << std::endl;

        //end time
        auto end_time = std::chrono::steady_clock::now();

        return 1;
    }

    if (!astRoot && !parseErrorOccurred) { // Check if root is null AND no parse error happened
        std::cerr << "Error: Parsing resulted in a null AST root without reporting specific parse errors." << std::endl;

        //end time
        auto end_time = std::chrono::steady_clock::now();

        return 1;
    }
    
    if (!parseErrorOccurred && astRoot) { // Check the flag AND if astRoot is valid
        std::cout << "Parsing completed successfully." << std::endl;
        std::cout << "Writing AST to: " << outputFilename << std::endl;
    
        // Serialize AST to JSON
        nlohmann::json astJson = astRoot->toJson();
    
        // Write JSON to output file
        std::ofstream outFile(outputFilename);
        if (!outFile) {
            std::cerr << "Error: Could not open output AST file: " << outputFilename << std::endl;

        //end time
        auto end_time = std::chrono::steady_clock::now();

            return 1;
        }
    
        // Write pretty-printed JSON
        outFile << std::setw(4) << astJson << std::endl;
        outFile.close();
    
        std::cout << "AST successfully written to " << outputFilename << std::endl;
    } else {
         std::cout << "Parsing finished with errors. AST was not generated or is incomplete." << std::endl;

        //end time
        auto end_time = std::chrono::steady_clock::now();

         return 1; // Indicate failure
    }

    //end time
    auto end_time = std::chrono::steady_clock::now();
    //calculate_time
    auto duration = end_time - start_time;


    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::cout << "Time execution: " << duration_ms.count() << " ms" << std::endl;

    return 0;
}