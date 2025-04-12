#include <iostream>
#include <vector>
#include <string>
#include "lexer.h"

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

// Test the lexer with incremental chunks to isolate errors
bool testLexerChunk(const std::string& description, const std::string& source) {
    std::cout << "\n=== TESTING: " << description << " ===\n";
    std::cout << "Source: \"" << source << "\"\n";
    
    try {
        Lexer lexer(source);
        
        int tokenCount = 0;
        const int MAX_TOKENS = 1000; // Limit to prevent infinite loops
        Token token;
        
        // Store the last position to detect if we're stuck
        int lastLine = -1;
        int lastColumn = -1;
        
        do {
            token = lexer.scanToken();
            printToken(token);
            tokenCount++;
            
            // Check if the lexer is stuck at the same position
            if (token.line == lastLine && token.column == lastColumn && 
                token.type != TokenType::EOF_TOKEN) {
                std::cerr << "ERROR: Lexer stuck at line " << token.line << ", column " 
                          << token.column << " (possible infinite loop)\n";
                return false;
            }
            
            lastLine = token.line;
            lastColumn = token.column;
            
            if (tokenCount >= MAX_TOKENS) {
                std::cerr << "ERROR: Too many tokens generated (possible infinite loop)\n";
                return false;
            }
            
        } while (token.type != TokenType::EOF_TOKEN);
        
        std::cout << "Successfully processed " << tokenCount << " tokens.\n";
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    // Test each part incrementally to isolate problems
    bool allPassed = true;
    
    // Test 1: Simple style includes
    allPassed &= testLexerChunk("Style includes", 
        "style <iostream>\nstyle <string>\n");
    
    // Test 2: Garden declaration
    allPassed &= testLexerChunk("Garden declaration", 
        "garden SimpleGarden\n");
    
    // Test 3: Start of species declaration
    allPassed &= testLexerChunk("Species declaration start", 
        "species Rose {\n");
    
    // Test 4: Open section
    allPassed &= testLexerChunk("Open section", 
        "open:\n    grow sayHello() -> void {\n");
    
    // Test 5: Function body
    allPassed &= testLexerChunk("Function body", 
        "        bloom << \"Hello from Hanami Rose!\\n\";\n"
        "        blossom;\n    }\n");
    
    // Test 6: Hidden section
    allPassed &= testLexerChunk("Hidden section", 
        "hidden:\n    int secretNumber = 42;\n");
    
    // Test 7: Guarded section
    allPassed &= testLexerChunk("Guarded section", 
        "guarded:\n    bool isFriendly = true;\n};");
    
    // Test 8: Main function start
    allPassed &= testLexerChunk("Main function start", 
        "grow mainGarden() -> int {\n"
        "    std::string userName;\n\n"
        "    bloom << \"What's your name? \";\n"
        "    water >> userName;\n");
    
    // Test 9: Object creation and method call
    allPassed &= testLexerChunk("Object creation", 
        "    Rose g;\n"
        "    g.sayHello();\n");
    
    // Test 10: Branch statements
    allPassed &= testLexerChunk("Branch statements", 
        "    branch (userName == \"Rose\") {\n"
        "        bloom << \"You have a lovely name!\\n\";\n"
        "    }\n"
        "    else branch (userName == \"Lily\") {\n"
        "        bloom << \"Another beautiful flower name!\\n\";\n"
        "    }\n"
        "    else {\n"
        "        bloom << \"Nice to meet you, \" << userName << \"!\\n\";\n"
        "    }\n");
    
    // Test 11: Function end
    allPassed &= testLexerChunk("Function end", 
        "    blossom 0;\n}");
    
    // Test 12: Full program (if the individual parts succeeded)
    if (allPassed) {
        const std::string fullProgram = 
            "style <iostream>\n"
            "style <string>\n\n"
            "garden SimpleGarden\n\n"
            "species Rose {\n"
            "open:\n"
            "    grow sayHello() -> void {\n"
            "        bloom << \"Hello from Hanami Rose!\\n\";\n"
            "        blossom;\n"
            "    }\n\n"
            "hidden:\n"
            "    int secretNumber = 42;\n\n"
            "guarded:\n"
            "    bool isFriendly = true;\n"
            "};\n\n"
            "grow mainGarden() -> int {\n"
            "    std::string userName;\n\n"
            "    bloom << \"What's your name? \";\n"
            "    water >> userName;\n\n"
            "    Rose g;\n"
            "    g.sayHello();\n\n"
            "    branch (userName == \"Rose\") {\n"
            "        bloom << \"You have a lovely name!\\n\";\n"
            "    }\n"
            "    else branch (userName == \"Lily\") {\n"
            "        bloom << \"Another beautiful flower name!\\n\";\n"
            "    }\n"
            "    else {\n"
            "        bloom << \"Nice to meet you, \" << userName << \"!\\n\";\n"
            "    }\n\n"
            "    blossom 0;\n"
            "}";
        
        std::cout << "\n=== TESTING FULL PROGRAM ===\n";
        testLexerChunk("Full Hanami Program", fullProgram);
    }
    
    // Final results
    if (allPassed) {
        std::cout << "\n=== ALL TESTS PASSED ===\n";
    } else {
        std::cout << "\n=== SOME TESTS FAILED ===\n";
        std::cout << "Check the output above to see where the lexer is failing.\n";
    }
    
    return allPassed ? 0 : 1;
}
