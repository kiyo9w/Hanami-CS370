#include "parser.h"

// ----- Trong parser.cpp -----

// Constructor
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

// Phương thức chính để phân tích cú pháp
std::unique_ptr<ASTNode> Parser::parse() {
    // Kiểm tra xem có token nào không trước khi bắt đầu
    if (tokens.empty() || tokens.back().type != TokenType::EOF_TOKEN) {
         // Có thể ném ngoại lệ hoặc xử lý lỗi ở đây nếu cần
         // Ví dụ: đảm bảo luôn có EOF_TOKEN ở cuối
         // throw std::runtime_error("Token stream is empty or does not end with EOF");
         // Hoặc đơn giản là trả về nullptr nếu ngữ cảnh cho phép
         if (tokens.empty()) return nullptr;
    }
    auto result = expression();
    // Tùy chọn: Kiểm tra xem đã tiêu thụ hết token chưa (ngoại trừ EOF)
    if (!isAtEnd() && peek().type != TokenType::EOF_TOKEN) {
        // Lỗi: Còn token thừa sau khi phân tích xong biểu thức chính
        // std::cerr << "Error: Unexpected token after expression: " << peek().lexeme << std::endl;
        // Có thể ném ngoại lệ hoặc trả về nullptr
        return nullptr; // Hoặc xử lý lỗi phù hợp
    }
    return result;
}


// Phương thức xem token hiện tại (nên là const)
const Token& Parser::peek() const {
    // Trả về token cuối cùng (EOF) nếu đã ở cuối để tránh lỗi
    if (current >= tokens.size()) {
       // Giả định tokens không rỗng và token cuối là EOF
       return tokens.back();
    }
    return tokens[current];
}

// *** ĐÂY LÀ PHẦN SỬA LỖI ***
// Phương thức để di chuyển đến token tiếp theo và trả về token *vừa được xử lý*
const Token& Parser::advance() {
    // Chỉ tăng chỉ số nếu chưa ở cuối (chưa trỏ vào EOF hoặc xa hơn)
    if (!isAtEnd()) {
        // Sử dụng post-increment: trả về tham chiếu const đến token tại vị trí 'current',
        // *sau đó* tăng 'current' lên 1.
        return tokens[current++];
    }
    // Nếu đã ở cuối, trả về token cuối cùng (thường là EOF) để tránh lỗi
    // và để các phương thức gọi không bị crash khi kiểm tra type.
    // Giả định rằng tokens không bao giờ rỗng và luôn có ít nhất EOF_TOKEN.
    return tokens.back();
}


// Kiểm tra xem đã hết token chưa (trỏ vào EOF hoặc xa hơn) (nên là const)
bool Parser::isAtEnd() const {
    // Coi là kết thúc nếu token hiện tại là EOF
    // Hoặc nếu chỉ số vượt quá kích thước (đề phòng trường hợp không có EOF)
    return current >= tokens.size() || tokens[current].type == TokenType::EOF_TOKEN;
    // Lưu ý: Cách triển khai isAtEnd() phụ thuộc vào cách Lexer tạo token.
    // Nếu Lexer *luôn* thêm EOF_TOKEN, chỉ cần kiểm tra type là đủ:
    // return peek().type == TokenType::EOF_TOKEN;
    // Nếu có thể không có EOF_TOKEN, kiểm tra chỉ số an toàn hơn:
    // return current >= tokens.size();
    // Cách kết hợp ở trên là an toàn nhất.
}

// Phương thức phân tích cú pháp biểu thức
std::unique_ptr<ASTNode> Parser::expression() {
    // Trong ngữ pháp đơn giản này, expression chỉ là term
    // Nếu sau này có gán (=), nó sẽ được xử lý ở đây.
    return term();
}

// Phương thức phân tích cú pháp phép cộng và trừ
std::unique_ptr<ASTNode> Parser::term() {
    auto node = factor();

    // Sử dụng peek() để kiểm tra trước khi advance()
    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        const Token& op_token = advance(); // Lấy token toán tử (+ hoặc -) và tiến tới
        auto right = factor();        // Phân tích toán hạng bên phải
        if (!right) {
            // Xử lý lỗi: thiếu toán hạng sau toán tử
             std::cerr << "Syntax Error: Expected expression after operator '" << op_token.lexeme << "' at line " << op_token.line << ", column " << op_token.column << std::endl;
             return nullptr; // Hoặc ném ngoại lệ
        }
        // Tạo node phép toán hai ngôi
        node = std::make_unique<BinOpNode>(std::move(node), op_token.type, std::move(right));
    }

    return node;
}

// Phương thức phân tích cú pháp phép nhân và chia
std::unique_ptr<ASTNode> Parser::factor() {
    auto node = primary();

    // Sử dụng peek() để kiểm tra trước khi advance()
    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        const Token& op_token = advance(); // Lấy token toán tử (* hoặc /) và tiến tới
        auto right = primary();       // Phân tích toán hạng bên phải
        if (!right) {
            // Xử lý lỗi: thiếu toán hạng sau toán tử
             std::cerr << "Syntax Error: Expected primary expression after operator '" << op_token.lexeme << "' at line " << op_token.line << ", column " << op_token.column << std::endl;
             return nullptr; // Hoặc ném ngoại lệ
        }
        // Tạo node phép toán hai ngôi
        node = std::make_unique<BinOpNode>(std::move(node), op_token.type, std::move(right));
    }

    return node;
}

// Phương thức phân tích cú pháp toán hạng cơ bản (số hoặc biến)
std::unique_ptr<ASTNode> Parser::primary() {
    // Sử dụng peek() để kiểm tra loại token hiện tại
    const Token& token = peek();

    if (token.type == TokenType::NUMBER) {
        advance(); // Tiêu thụ token số
        try {
            // Chuyển đổi string sang int, có thể ném lỗi nếu không hợp lệ
            return std::make_unique<NumberNode>(std::stoi(token.lexeme));
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Error: Invalid number format '" << token.lexeme << "' at line " << token.line << ", column " << token.column << std::endl;
            return nullptr;
        } catch (const std::out_of_range& oor) {
             std::cerr << "Error: Number out of range '" << token.lexeme << "' at line " << token.line << ", column " << token.column << std::endl;
            return nullptr;
        }
    } else if (token.type == TokenType::IDENTIFIER) {
        advance(); // Tiêu thụ token định danh
        return std::make_unique<IdentifierNode>(token.lexeme);
    }
    // Thêm xử lý cho dấu ngoặc đơn (nếu ngữ pháp yêu cầu)
    // else if (token.type == TokenType::LPAREN) {
    //     advance(); // Tiêu thụ '('
    //     auto expr = expression(); // Phân tích biểu thức bên trong ngoặc
    //     if (peek().type == TokenType::RPAREN) {
    //         advance(); // Tiêu thụ ')'
    //         return expr;
    //     } else {
    //         // Lỗi: Thiếu dấu ngoặc đóng
    //         std::cerr << "Syntax Error: Expected ')' after expression at line " << peek().line << ", column " << peek().column << std::endl;
    //         return nullptr;
    //     }
    // }
     else {
        // Lỗi: Token không mong đợi
        // Chỉ báo lỗi nếu đó không phải là EOF được mong đợi ở cuối
        if (token.type != TokenType::EOF_TOKEN) {
           std::cerr << "Syntax Error: Unexpected token '" << token.lexeme << "' at line " << token.line << ", column " << token.column << ". Expected number or identifier." << std::endl;
        }
        return nullptr; // Trả về nullptr để báo hiệu lỗi phân tích cú pháp
    }
}