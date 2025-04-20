#ifndef JSON_DESERIALIZER_H
#define JSON_DESERIALIZER_H

#include <memory>
#include "../common/json.hpp"

// Forward declare AST nodes instead of including ast.h directly
// This avoids circular dependencies if ast.h needed json utils in the future
struct ASTNode;
struct Expression;
struct Statement;
struct BlockStmt;
struct VisibilityBlockStmt;
struct IfBranch;
struct WhileStmt;
struct ForStmt;

// --- Deserialization Function Declarations --- 

std::unique_ptr<ASTNode> fromJson(const nlohmann::json& j);

// Internal helpers (could be in .cpp if not needed outside)
std::unique_ptr<Expression> expressionFromJson(const nlohmann::json& j);
std::unique_ptr<Statement> statementFromJson(const nlohmann::json& j);
std::unique_ptr<BlockStmt> blockStmtFromJson(const nlohmann::json& j);
std::unique_ptr<VisibilityBlockStmt> visibilityBlockFromJson(const nlohmann::json& j);
IfBranch ifBranchFromJson(const nlohmann::json& j);

// Forward declare helpers if needed, though likely handled within statementFromJson
// std::unique_ptr<WhileStmt> whileStmtFromJson(const nlohmann::json& j);
// std::unique_ptr<ForStmt> forStmtFromJson(const nlohmann::json& j);

#endif // JSON_DESERIALIZER_H 