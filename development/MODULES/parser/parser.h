#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream> // For error reporting
#include "json.hpp"
#include <functional>

#include "../common/token.h"

// Forward declarations
struct ASTNode;
struct Expression;
struct Statement;

// --- Error Handling --- 

// Custom exception class for parsing errors
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

// --- AST Node Base ---
struct ASTNode {
    virtual ~ASTNode() = default;
    // Basic JSON serialization (can be overridden by derived classes)
    virtual nlohmann::json toJson() const {
        nlohmann::json j;
        j["node_type"] = "ASTNode"; // Default type
        return j;
    }
};

// --- Expressions ---
struct Expression : public ASTNode {
    nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "Expression";
        return j;
    }
};

struct IdentifierExpr : public Expression {
    std::string name;
    IdentifierExpr(std::string n) : name(std::move(n)) {}
    nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "IdentifierExpr";
        j["name"] = name;
        return j;
    }
};

struct NumberLiteralExpr : public Expression {
    std::string value; // Store as string to handle potential float/int variations initially
    NumberLiteralExpr(std::string v) : value(std::move(v)) {}
    nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "NumberLiteralExpr";
        j["value"] = value;
        return j;
    }
};

struct StringLiteralExpr : public Expression {
    std::string value;
    StringLiteralExpr(std::string v) : value(std::move(v)) {}
    nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "StringLiteralExpr";
        j["value"] = value;
        return j;
    }
};

struct BooleanLiteralExpr : public Expression {
    bool value;
    BooleanLiteralExpr(bool v) : value(v) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "BooleanLiteralExpr";
        j["value"] = value;
        return j;
    }
};

struct BinaryOpExpr : public Expression {
    TokenType op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    BinaryOpExpr(TokenType o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "BinaryOpExpr";
        // TODO: Convert op TokenType to string representation if needed for JSON
        j["operator"] = static_cast<int>(op); // Simple int representation for now
        j["left"] = left ? left->toJson() : nullptr;
        j["right"] = right ? right->toJson() : nullptr;
        return j;
    }
};

struct FunctionCallExpr : public Expression {
    std::unique_ptr<Expression> callee; // Could be IdentifierExpr or MemberAccessExpr
    std::vector<std::unique_ptr<Expression>> arguments;
    FunctionCallExpr(std::unique_ptr<Expression> c) : callee(std::move(c)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "FunctionCallExpr";
        j["callee"] = callee ? callee->toJson() : nullptr;
        j["arguments"] = nlohmann::json::array();
        for(const auto& arg : arguments) {
            j["arguments"].push_back(arg ? arg->toJson() : nullptr);
        }
        return j;
    }
};

struct MemberAccessExpr : public Expression {
    std::unique_ptr<Expression> object; // e.g., 'g'
    std::unique_ptr<IdentifierExpr> member; // e.g., 'sayHello'
    MemberAccessExpr(std::unique_ptr<Expression> obj, std::unique_ptr<IdentifierExpr> mem)
        : object(std::move(obj)), member(std::move(mem)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "MemberAccessExpr";
        j["object"] = object ? object->toJson() : nullptr;
        j["member"] = member ? member->toJson() : nullptr;
        return j;
    }
};

// --- Statements ---
struct Statement : public ASTNode {
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "Statement";
        return j;
    }
};

struct ProgramNode : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "ProgramNode";
        j["statements"] = nlohmann::json::array();
        for(const auto& stmt : statements) {
            j["statements"].push_back(stmt ? stmt->toJson() : nullptr);
        }
        return j;
    }
};

struct StyleIncludeStmt : public Statement {
    std::string path;
    StyleIncludeStmt(std::string p) : path(std::move(p)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "StyleIncludeStmt";
        j["path"] = path;
        return j;
    }
};

struct GardenDeclStmt : public Statement {
    std::string name;
    GardenDeclStmt(std::string n) : name(std::move(n)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "GardenDeclStmt";
        j["name"] = name;
        return j;
    }
};

struct BlockStmt : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "BlockStmt";
        j["statements"] = nlohmann::json::array();
        for(const auto& stmt : statements) {
            j["statements"].push_back(stmt ? stmt->toJson() : nullptr);
        }
        return j;
    }
};


struct VisibilityBlockStmt : public Statement {
    TokenType visibility; // OPEN, HIDDEN, GUARDED
    std::unique_ptr<BlockStmt> block;
    VisibilityBlockStmt(TokenType v, std::unique_ptr<BlockStmt> b) : visibility(v), block(std::move(b)) {}
    nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "VisibilityBlockStmt";
        // TODO: Convert visibility TokenType to string
        j["visibility"] = static_cast<int>(visibility);
        j["block"] = block ? block->toJson() : nullptr;
        return j;
    }
};


struct SpeciesDeclStmt : public Statement {
    std::string name;
    std::vector<std::unique_ptr<VisibilityBlockStmt>> sections; // open:, hidden:, guarded:
    SpeciesDeclStmt(std::string n) : name(std::move(n)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "SpeciesDeclStmt";
        j["name"] = name;
        j["sections"] = nlohmann::json::array();
         for(const auto& section : sections) {
            j["sections"].push_back(section ? section->toJson() : nullptr);
        }
        return j;
    }
};


struct VariableDeclStmt : public Statement {
    std::string typeName; // e.g., "int", "bool", "std::string", "Rose"
    std::string varName;
    std::unique_ptr<Expression> initializer; // Optional
    VariableDeclStmt(std::string type, std::string name, std::unique_ptr<Expression> init = nullptr)
        : typeName(std::move(type)), varName(std::move(name)), initializer(std::move(init)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "VariableDeclStmt";
        j["typeName"] = typeName;
        j["varName"] = varName;
        j["initializer"] = initializer ? initializer->toJson() : nullptr;
        return j;
    }
};

struct AssignmentStmt : public Expression {
    std::unique_ptr<Expression> left; // Usually IdentifierExpr or MemberAccessExpr
    std::unique_ptr<Expression> right;
    AssignmentStmt(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}
    nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "AssignmentStmt";
        j["left"] = left ? left->toJson() : nullptr;
        j["right"] = right ? right->toJson() : nullptr;
        return j;
    }
};


struct Parameter {
    std::string typeName;
    std::string paramName;
    Parameter(std::string type, std::string name) : typeName(std::move(type)), paramName(std::move(name)) {}
    nlohmann::json toJson() const {
         nlohmann::json j;
         j["typeName"] = typeName;
         j["paramName"] = paramName;
         return j;
    }
};


struct FunctionDefStmt : public Statement {
    std::string name;
    std::vector<Parameter> parameters;
    std::string returnType;
    std::unique_ptr<BlockStmt> body;
    FunctionDefStmt(std::string n, std::string retType, std::unique_ptr<BlockStmt> b)
        : name(std::move(n)), returnType(std::move(retType)), body(std::move(b)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "FunctionDefStmt";
        j["name"] = name;
        j["returnType"] = returnType;
        j["parameters"] = nlohmann::json::array();
        for(const auto& param : parameters) {
            j["parameters"].push_back(param.toJson());
        }
        j["body"] = body ? body->toJson() : nullptr;
        return j;
    }
};

struct ReturnStmt : public Statement { // 'blossom' keyword
    std::unique_ptr<Expression> returnValue; // Optional
    ReturnStmt(std::unique_ptr<Expression> val = nullptr) : returnValue(std::move(val)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "ReturnStmt";
        j["returnValue"] = returnValue ? returnValue->toJson() : nullptr;
        return j;
    }
};

struct ExpressionStmt : public Statement {
    std::unique_ptr<Expression> expression;
    ExpressionStmt(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "ExpressionStmt";
        j["expression"] = expression ? expression->toJson() : nullptr;
        return j;
    }
};


struct IfBranch {
     std::unique_ptr<Expression> condition; // Null for 'else' block
     std::unique_ptr<BlockStmt> body;
     IfBranch(std::unique_ptr<Expression> cond, std::unique_ptr<BlockStmt> b)
         : condition(std::move(cond)), body(std::move(b)) {}
     nlohmann::json toJson() const {
         nlohmann::json j;
         j["condition"] = condition ? condition->toJson() : nullptr;
         j["body"] = body ? body->toJson() : nullptr;
         return j;
     }
};

struct BranchStmt : public Statement { // 'branch' / 'else branch' / 'else'
    std::vector<IfBranch> branches;
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "BranchStmt";
        j["branches"] = nlohmann::json::array();
        for(const auto& branch : branches) {
            j["branches"].push_back(branch.toJson());
        }
        return j;
    }
};

struct IOStmt : public Statement { // 'bloom <<' or 'water >>'
    TokenType ioType; // BLOOM or WATER
    TokenType direction; // STREAM_OUT or STREAM_IN
    std::vector<std::unique_ptr<Expression>> expressions; // Multiple expressions possible with << or >>
    IOStmt(TokenType type, TokenType dir) : ioType(type), direction(dir) {}
     nlohmann::json toJson() const override {
        nlohmann::json j;
        j["node_type"] = "IOStmt";
        // TODO: Convert ioType and direction TokenType to string
        j["ioType"] = static_cast<int>(ioType);
        j["direction"] = static_cast<int>(direction);
        j["expressions"] = nlohmann::json::array();
         for(const auto& expr : expressions) {
            j["expressions"].push_back(expr ? expr->toJson() : nullptr);
        }
        return j;
    }
};


// --- Parser Class ---
class Parser {
    public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ProgramNode> parse();
    
    private:
        const std::vector<Token>& tokens;
    size_t current = 0;
    
    // Helper methods
        const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize(); // Error recovery

    // Parsing methods for grammar rules
    std::unique_ptr<Statement> parseDeclaration();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseStyleInclude();
    std::unique_ptr<Statement> parseGardenDeclaration();
    std::unique_ptr<Statement> parseSpeciesDeclaration();
    std::unique_ptr<VisibilityBlockStmt> parseVisibilityBlock();
    std::unique_ptr<BlockStmt> parseBlock();
    std::unique_ptr<Statement> parseFunctionDefinition();
    std::unique_ptr<Statement> parseVariableDeclarationOrExprStmt();
    std::unique_ptr<Statement> parseBranchStatement();
    std::unique_ptr<Statement> parseIOStatement(TokenType ioType);
    std::unique_ptr<Statement> parseReturnStatement(); // blossom
    std::unique_ptr<Statement> parseExpressionStatement();


    // Expression parsing (following operator precedence)
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAssignment(); // Handles assignment (=)
    std::unique_ptr<Expression> parseLogicalOr();  // Handles ||
    std::unique_ptr<Expression> parseLogicalAnd(); // Handles &&
    std::unique_ptr<Expression> parseEquality();   // Handles ==, !=
    std::unique_ptr<Expression> parseComparison(); // Handles <, >, <=, >=
    std::unique_ptr<Expression> parseTerm();       // Handles +, -
    std::unique_ptr<Expression> parseFactor();     // Handles *, /, %
    std::unique_ptr<Expression> parseUnary();      // Handles !, - (unary)
    std::unique_ptr<Expression> parseCall();       // Handles function calls like `expr()` and member access like `expr.member`
    std::unique_ptr<Expression> parsePrimary();    // Handles literals, grouping, identifiers

    // Error reporting
    void error(const Token& token, const std::string& message);

    // Static helper functions
    static std::unique_ptr<Expression> parseBinaryHelper(
        Parser* parser,
        std::function<std::unique_ptr<Expression>()> parseOperand,
        const std::vector<TokenType>& operators
    );
    static std::unique_ptr<Expression> finishCall(Parser* parser, std::unique_ptr<Expression> callee);

};

#endif // PARSER_H 