#include "lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

// Helper function to convert TokenType to string for display
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        // Keywords
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::RETURN: return "RETURN";
        case TokenType::FUNCTION: return "FUNCTION";
        
        // Literals
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        
        // Operators
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        
        // Punctuation
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::DOT: return "DOT";
        
        // Special
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::ERROR: return "ERROR";
        
        default: return "UNKNOWN";
    }
}

// Function to run lexer on provided source and print all tokens
void testLexer(const std::string& source, const std::string& testName) {
    std::cout << "\n=== " << testName << " ===\n";
    std::cout << "SOURCE:\n" << source << "\n\n";
    
    Lexer lexer(source);
    
    std::cout << std::left << std::setw(15) << "TYPE" 
              << std::setw(20) << "LEXEME" 
              << std::setw(8) << "LINE" 
              << std::setw(8) << "COLUMN" << "\n";
    std::cout << std::string(50, '-') << "\n";
    
    while (true) {
        Token token = lexer.scanToken();
        std::cout << std::left << std::setw(15) << tokenTypeToString(token.type) 
                  << std::setw(20) << token.lexeme 
                  << std::setw(8) << token.line 
                  << std::setw(8) << token.column << "\n";
                  
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
    }
    std::cout << "\n";
}

int main() {
    // Test 1: Basic tokens
    testLexer("( ) { } [ ] , . ; + - * / = == != < > && ||", "Basic Tokens");
    
    // Test 2: Keywords
    testLexer("if else while for return function", "Keywords");
    
    // Test 3: Literals
    testLexer("identifier _private camelCase snake_case NUM1 x y z true false", "Identifiers and Booleans");
    
    // Test 4: Numbers
    testLexer("123 3.14159 0.5 42.0 0", "Numbers");
    
    // Test 5: Strings
    testLexer("\"Hello, World!\" \"This is a test string\" \"Special chars: \\n\\t\\\\\"", "Strings");
    
    // Test 6: Single line comments
    testLexer("// This is a comment\nx = 5; // This is another comment", "Single Line Comments");
    
    // Test 7: Multi-line comments
    testLexer("/* This is a\nmulti-line\ncomment */\ny = 10;", "Multi-line Comments");
    
    // Test 8: Mixed code
    testLexer("function test(x, y) {\n  if (x > y) {\n    return x;\n  } else {\n    return y;\n  }\n}", 
              "Mixed Code");
    
    // Test 9: Error cases
    testLexer("@#$%^&", "Error Cases");
    
    // Test 10: Line and column tracking
    testLexer("if (x == 10) {\n    return true;\n}", "Line and Column Tracking");
    
    // Test 11: Complex example
    std::string complexExample = R"(
function Calculator() {
  this.result = 0;

  // Add two numbers
  this.add = function(a, b) {
    return a + b; // Return the sum
  };

  /* Multiply 
     two numbers */
  this.multiply = function(a, b) {
    return a * b;
  };
}

var calc = new Calculator();
var sum = calc.add(5, 10);
if (sum > 10) {
  return true;
} else {
  return false;
}
)";
    testLexer(complexExample, "Complex Example");
    
    return 0;
}
