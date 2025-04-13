#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "json.hpp" // Include json here as nodes use it
#include <functional> // Needed for Parser helpers if they stay here, maybe move later

#include "token.h" // Depends on TokenType

// --- Error Handling --- 

// Custom exception class for parsing errors (Potentially needed by deserializer?)
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

// --- AST Node Base --- 
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual nlohmann::json toJson() const {
        nlohmann::json j;
        j["node_type"] = "ASTNode"; 
        return j;
    }
    // Optional: Add field for semantic analysis results (e.g., DataType)
    // DataType semantic_type = UNKNOWN_TYPE; // Example
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
    std::string value; 
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
        // TODO: Convert op TokenType to string 
        j["operator"] = static_cast<int>(op); 
        j["left"] = left ? left->toJson() : nullptr;
        j["right"] = right ? right->toJson() : nullptr;
        return j;
    }
};

struct FunctionCallExpr : public Expression {
    std::unique_ptr<Expression> callee; 
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
    std::unique_ptr<Expression> object; 
    std::unique_ptr<IdentifierExpr> member;
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
    TokenType visibility; 
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
    std::vector<std::unique_ptr<VisibilityBlockStmt>> sections; 
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
    std::string typeName; 
    std::string varName;
    std::unique_ptr<Expression> initializer; 
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
    std::unique_ptr<Expression> left; 
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

struct ReturnStmt : public Statement { 
    std::unique_ptr<Expression> returnValue; 
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
     std::unique_ptr<Expression> condition; 
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

struct BranchStmt : public Statement { 
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

struct IOStmt : public Statement { 
    TokenType ioType; 
    TokenType direction; 
    std::vector<std::unique_ptr<Expression>> expressions; 
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


#endif // AST_H 