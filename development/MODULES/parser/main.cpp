#include "token.h" // Đảm bảo bao gồm định nghĩa Token, ASTNode,...
#include "parser.h"
#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept> // Để sử dụng std::runtime_error (nếu cần)

// Hàm trợ giúp để in AST (ví dụ đơn giản)
void printAST(const ASTNode* node, int indent = 0) {
    if (!node) {
        std::cout << std::string(indent, ' ') << "(null)" << std::endl;
        return;
    }

    // Sử dụng dynamic_cast để xác định loại node
    if (const BinOpNode* binOp = dynamic_cast<const BinOpNode*>(node)) {
        std::cout << std::string(indent, ' ') << "BinOp: ";
        switch(binOp->op) {
            case TokenType::PLUS: std::cout << "+"; break;
            case TokenType::MINUS: std::cout << "-"; break;
            case TokenType::STAR: std::cout << "*"; break;
            case TokenType::SLASH: std::cout << "/"; break;
            // Thêm các toán tử khác nếu có
            default: std::cout << "?"; break;
        }
        std::cout << std::endl;
        printAST(binOp->left.get(), indent + 2);
        printAST(binOp->right.get(), indent + 2);
    } else if (const NumberNode* numNode = dynamic_cast<const NumberNode*>(node)) {
        std::cout << std::string(indent, ' ') << "Number: " << numNode->value << std::endl;
    } else if (const IdentifierNode* idNode = dynamic_cast<const IdentifierNode*>(node)) {
        std::cout << std::string(indent, ' ') << "Identifier: " << idNode->name << std::endl;
    } else {
        std::cout << std::string(indent, ' ') << "Unknown Node" << std::endl;
    }
}


int main() {
    // Ví dụ 1: Biểu thức đơn giản
    std::cout << "--- Parsing: 3 + 4 ---" << std::endl;
    std::vector<Token> tokens1 = {
        {TokenType::NUMBER, "3", 1, 1},
        {TokenType::PLUS, "+", 1, 3},
        {TokenType::NUMBER, "4", 1, 5},
        {TokenType::EOF_TOKEN, "", 1, 6} // Luôn cần EOF
    };

    Parser parser1(tokens1);
    auto ast1 = parser1.parse();

    if (ast1) {
        std::cout << "Parsing successful!" << std::endl;
        std::cout << "Generated AST:" << std::endl;
        printAST(ast1.get());
    } else {
        std::cout << "Parsing failed." << std::endl;
    }
    std::cout << std::endl;


    // Ví dụ 2: Biểu thức phức tạp hơn
     std::cout << "--- Parsing: 10 * x - 5 ---" << std::endl;
    std::vector<Token> tokens2 = {
        {TokenType::NUMBER, "10", 1, 1},
        {TokenType::STAR, "*", 1, 4},
        {TokenType::IDENTIFIER, "x", 1, 6},
        {TokenType::MINUS, "-", 1, 8},
        {TokenType::NUMBER, "5", 1, 10},
        {TokenType::EOF_TOKEN, "", 1, 11}
    };

    Parser parser2(tokens2);
    auto ast2 = parser2.parse();

    if (ast2) {
        std::cout << "Parsing successful!" << std::endl;
        std::cout << "Generated AST:" << std::endl;
        printAST(ast2.get());
    } else {
        std::cout << "Parsing failed." << std::endl;
    }
     std::cout << std::endl;

    // Ví dụ 3: Lỗi cú pháp (thiếu toán hạng)
    std::cout << "--- Parsing: 7 + ---" << std::endl;
    std::vector<Token> tokens3 = {
        {TokenType::NUMBER, "7", 1, 1},
        {TokenType::PLUS, "+", 1, 3},
        {TokenType::EOF_TOKEN, "", 1, 4}
    };

    Parser parser3(tokens3);
    auto ast3 = parser3.parse(); // Sẽ in lỗi từ bên trong parser

    if (ast3) {
        std::cout << "Parsing successful!" << std::endl;
         printAST(ast3.get());
    } else {
        std::cout << "Parsing failed as expected." << std::endl;
    }
     std::cout << std::endl;


    return 0;
}