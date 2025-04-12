#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <memory>
#include <iostream> // Để sử dụng std::cerr cho thông báo lỗi
#include <stdexcept> // Để sử dụng std::runtime_error (nếu cần)


// Định nghĩa các loại token
enum class TokenType {
    NUMBER,
    IDENTIFIER,
    ASSIGN,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    EQUAL,
    EOF_TOKEN,
    ERROR
};

// Cấu trúc token
struct Token {
    TokenType type;
    std::string lexeme;
    int line, column;
};

// Định nghĩa các node trong AST
struct ASTNode {
    virtual ~ASTNode() = default;
};

struct BinOpNode : public ASTNode {
    std::unique_ptr<ASTNode> left, right;
    TokenType op;

    BinOpNode(std::unique_ptr<ASTNode> l, TokenType o, std::unique_ptr<ASTNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

struct NumberNode : public ASTNode {
    int value;

    NumberNode(int v) : value(v) {}
};

struct IdentifierNode : public ASTNode {
    std::string name;

    IdentifierNode(const std::string& n) : name(n) {}
};

#endif  // TOKEN_H
