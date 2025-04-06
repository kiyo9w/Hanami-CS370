#include "token.h"
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept> // Để sử dụng std::runtime_error (nếu cần)


class Parser {
    public:
        explicit Parser(const std::vector<Token>& tokens);
        std::unique_ptr<ASTNode> parse();
    
    private:
        const std::vector<Token>& tokens;
        size_t current;
    
        // Sửa khai báo: trả về const Token&
        const Token& peek() const;
        const Token& advance(); // Sửa: trả về const Token&
        bool isAtEnd() const; // Nên là const vì không thay đổi trạng thái
    
        std::unique_ptr<ASTNode> expression();
        std::unique_ptr<ASTNode> term();
        std::unique_ptr<ASTNode> factor();
        std::unique_ptr<ASTNode> primary();
    };