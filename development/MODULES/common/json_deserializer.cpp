#include "json_deserializer.h"
#include <stdexcept>
#include <iostream>
#include "ast.h"
#include "utils.h"

// --- JSON Deserialization Implementation --- 

std::unique_ptr<Expression> expressionFromJson(const nlohmann::json& j) {
    if (!j.is_object() || !j.contains("node_type")) return nullptr;
    std::string node_type = j["node_type"].get<std::string>();

    try {
        if (node_type == "IdentifierExpr") {
            return std::make_unique<IdentifierExpr>(j.at("name").get<std::string>());
        } else if (node_type == "NumberLiteralExpr") {
            std::string val_str = j.value("value", ""); 
            return std::make_unique<NumberLiteralExpr>(val_str);
        } else if (node_type == "StringLiteralExpr") {
            return std::make_unique<StringLiteralExpr>(j.at("value").get<std::string>());
        } else if (node_type == "BooleanLiteralExpr") {
            return std::make_unique<BooleanLiteralExpr>(j.at("value").get<bool>());
        } else if (node_type == "BinaryOpExpr") {
            auto left = expressionFromJson(j.at("left"));
            auto right = expressionFromJson(j.at("right"));
            std::string opStr = j.at("operator").get<std::string>();
            TokenType op = stringToTokenType(opStr); 
            return std::make_unique<BinaryOpExpr>(op, std::move(left), std::move(right));
        } else if (node_type == "FunctionCallExpr") {
            auto callee = expressionFromJson(j.at("callee"));
            auto callExpr = std::make_unique<FunctionCallExpr>(std::move(callee));
            if (j.contains("arguments") && j.at("arguments").is_array()) {
                for (const auto& argJson : j["arguments"]) {
                    callExpr->arguments.push_back(expressionFromJson(argJson));
                }
            }
            return callExpr;
        } else if (node_type == "MemberAccessExpr") {
            auto object = expressionFromJson(j.at("object"));
            auto member = expressionFromJson(j.at("member")); 
            IdentifierExpr* identMember = dynamic_cast<IdentifierExpr*>(member.release());
            if (!identMember) {
                 throw std::runtime_error("MemberAccessExpr member must be an IdentifierExpr");
            }
            return std::make_unique<MemberAccessExpr>(std::move(object), 
                       std::unique_ptr<IdentifierExpr>(identMember));
        } else if (node_type == "AssignmentStmt") { 
             auto left = expressionFromJson(j.at("left"));
             auto right = expressionFromJson(j.at("right"));
             return std::make_unique<AssignmentStmt>(std::move(left), std::move(right));
        }
        // --- Add other expression types defined in ast.h --- 
         else if (node_type == "Expression") { // Base class, shouldn't be instantiated directly usually
             std::cerr << "Warning: Deserializing base 'Expression' node type." << std::endl;
             return std::make_unique<Expression>();
         }
        // ... 
        
        throw std::runtime_error("Unknown or unhandled expression node_type: " + node_type);

    } catch (const nlohmann::json::exception& e) {
         std::cerr << "JSON access error during expression deserialization ('" << node_type << "'): " << e.what() << std::endl;
         return nullptr;
     } catch (const std::exception& e) {
          std::cerr << "Error during expression deserialization ('" << node_type << "'): " << e.what() << std::endl;
         return nullptr;
     }
}

std::unique_ptr<BlockStmt> blockStmtFromJson(const nlohmann::json& j) {
    if (!j.is_object() || !j.contains("node_type") || j.at("node_type") != "BlockStmt") {
         throw std::runtime_error("Invalid JSON for BlockStmt deserialization.");
    }
    auto block = std::make_unique<BlockStmt>();
    if (j.contains("statements") && j.at("statements").is_array()) {
        for (const auto& stmtJson : j["statements"]) {
            auto statement = statementFromJson(stmtJson); // Use the main statement deserializer
            if (statement) { // Only add if successfully deserialized
                 block->statements.push_back(std::move(statement));
            } else {
                 std::cerr << "Warning: Failed to deserialize a statement within BlockStmt." << std::endl;
            }
        }
    }
    return block;
}

std::unique_ptr<VisibilityBlockStmt> visibilityBlockFromJson(const nlohmann::json& j) {
     if (!j.is_object() || !j.contains("node_type") || j.at("node_type") != "VisibilityBlockStmt") {
         throw std::runtime_error("Invalid JSON for VisibilityBlockStmt deserialization.");
     }
     std::string visStr = j.at("visibility").get<std::string>();
     TokenType visibility = stringToTokenType(visStr);
     auto block = blockStmtFromJson(j.at("block"));
     return std::make_unique<VisibilityBlockStmt>(visibility, std::move(block));
}

IfBranch ifBranchFromJson(const nlohmann::json& j) {
     if (!j.is_object()) {
         throw std::runtime_error("Invalid JSON for IfBranch deserialization.");
     }
     std::unique_ptr<Expression> condition = nullptr;
     // Use .value() to handle potentially null condition safely
     if(!j.value("condition", nlohmann::json()).is_null()) {
         condition = expressionFromJson(j.at("condition"));
     }
     auto body = blockStmtFromJson(j.at("body"));
     return IfBranch(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> statementFromJson(const nlohmann::json& j) {
    if (!j.is_object() || !j.contains("node_type")) return nullptr;
    std::string node_type = j.at("node_type").get<std::string>();

    try {
        if (node_type == "ProgramNode") {
            throw std::runtime_error("ProgramNode found within statement list during deserialization.");
        }
        if (node_type == "StyleIncludeStmt") {
            return std::make_unique<StyleIncludeStmt>(j.at("path").get<std::string>());
        }
        if (node_type == "GardenDeclStmt") {
             return std::make_unique<GardenDeclStmt>(j.at("name").get<std::string>());
        }
        if (node_type == "BlockStmt") {
            return blockStmtFromJson(j);
        }
         if (node_type == "VisibilityBlockStmt") {
             throw std::runtime_error("VisibilityBlockStmt found outside SpeciesDeclStmt during deserialization.");
         }
         if (node_type == "SpeciesDeclStmt") {
              auto species = std::make_unique<SpeciesDeclStmt>(j.at("name").get<std::string>());
               if (j.contains("sections") && j.at("sections").is_array()) {
                   for (const auto& sectionJson : j["sections"]) {
                        species->sections.push_back(visibilityBlockFromJson(sectionJson));
                   }
               }
               return species;
         }
         if (node_type == "VariableDeclStmt") {
              std::unique_ptr<Expression> initializer = nullptr;
              if (!j.value("initializer", nlohmann::json()).is_null()) {
                  initializer = expressionFromJson(j.at("initializer"));
              }
              return std::make_unique<VariableDeclStmt>(j.at("typeName").get<std::string>(), 
                                                      j.at("varName").get<std::string>(), 
                                                      std::move(initializer));
         }
          if (node_type == "AssignmentStmt") { 
               // Assignment is an expression, handle via ExpressionStmt
               auto left = expressionFromJson(j.at("left"));
               auto right = expressionFromJson(j.at("right"));
               auto assignExpr = std::make_unique<AssignmentStmt>(std::move(left), std::move(right));
               return std::make_unique<ExpressionStmt>(std::move(assignExpr));
          }
          if (node_type == "FunctionDefStmt") {
               std::unique_ptr<BlockStmt> blockBody = nullptr;
               if (j.contains("body") && j.at("body").is_object()) {
                   blockBody = blockStmtFromJson(j.at("body"));
               } else {
                    throw std::runtime_error("Function definition body is missing or not a BlockStmt.");
               }
               
               auto func = std::make_unique<FunctionDefStmt>(j.at("name").get<std::string>(), 
                                                           j.at("returnType").get<std::string>(),
                                                           std::move(blockBody));
                                                           
                if (j.contains("parameters") && j.at("parameters").is_array()) {
                     for (const auto& paramJson : j["parameters"]) {
                          func->parameters.emplace_back(paramJson.at("typeName").get<std::string>(), 
                                                      paramJson.at("paramName").get<std::string>());
                     }
                }
                return func;
          }
           if (node_type == "ReturnStmt") {
                std::unique_ptr<Expression> returnValue = nullptr;
                 if (!j.value("returnValue", nlohmann::json()).is_null()) {
                     returnValue = expressionFromJson(j.at("returnValue"));
                 }
                return std::make_unique<ReturnStmt>(std::move(returnValue));
           }
           if (node_type == "ExpressionStmt") {
               return std::make_unique<ExpressionStmt>(expressionFromJson(j.at("expression")));
           }
            if (node_type == "BranchStmt") {
                 auto branchStmt = std::make_unique<BranchStmt>();
                  if (j.contains("branches") && j.at("branches").is_array()) {
                      for (const auto& branchJson : j["branches"]) {
                          branchStmt->branches.push_back(ifBranchFromJson(branchJson));
                      }
                  }
                  return branchStmt;
            }
             if (node_type == "IOStmt") {
                  std::string ioTypeStr = j.at("ioType").get<std::string>();
                  std::string directionStr = j.at("direction").get<std::string>();
                  TokenType ioType = stringToTokenType(ioTypeStr);
                  TokenType direction = stringToTokenType(directionStr);
                  auto ioStmt = std::make_unique<IOStmt>(ioType, direction);
                  if (j.contains("expressions") && j.at("expressions").is_array()) {
                      for (const auto& exprJson : j["expressions"]) {
                          ioStmt->expressions.push_back(expressionFromJson(exprJson));
                      }
                  }
                  return ioStmt;
             }
        // --- Add other statement types --- 
         else if (node_type == "Statement") { // Base class
              std::cerr << "Warning: Deserializing base 'Statement' node type." << std::endl;
             return std::make_unique<Statement>();
         }
        // --- Fallback for Expressions used as Statements --- 
        auto expr = expressionFromJson(j); 
        if(expr) {
            if (node_type == "FunctionCallExpr" || node_type == "MemberAccessExpr") {
                 return std::make_unique<ExpressionStmt>(std::move(expr));
            } else {
                 throw std::runtime_error("Unhandled expression type ('" + node_type + "') found where statement expected.");
            }
        }
        
        throw std::runtime_error("Unknown or unhandled statement node_type: " + node_type);
        
     } catch (const nlohmann::json::exception& e) {
         std::cerr << "JSON access error during statement deserialization ('" << node_type << "'): " << e.what() << std::endl;
         return nullptr;
     } catch (const std::exception& e) {
          std::cerr << "Error during statement deserialization ('" << node_type << "'): " << e.what() << std::endl;
         return nullptr;
     }
}

std::unique_ptr<ASTNode> fromJson(const nlohmann::json& j) {
     if (!j.is_object() || !j.contains("node_type")) return nullptr;
     std::string node_type = j.at("node_type").get<std::string>();

    try {
         if (node_type == "ProgramNode") {
              auto program = std::make_unique<ProgramNode>();
              if (j.contains("statements") && j.at("statements").is_array()) {
                 for (const auto& stmtJson : j["statements"]) {
                     auto statement = statementFromJson(stmtJson);
                     if(statement) { 
                        program->statements.push_back(std::move(statement));
                     } else {
                          std::cerr << "Warning: Failed to deserialize a statement within ProgramNode." << std::endl;
                     }
                 }
              }
             return program;
         }
     
         throw std::runtime_error("Expected 'ProgramNode' at the top level of AST JSON, found: " + node_type);

     } catch (const nlohmann::json::exception& e) {
         std::cerr << "JSON access error during top-level deserialization ('" << node_type << "'): " << e.what() << std::endl;
         return nullptr;
     } catch (const std::exception& e) {
          std::cerr << "Error during top-level deserialization ('" << node_type << "'): " << e.what() << std::endl;
         return nullptr;
     }
} 