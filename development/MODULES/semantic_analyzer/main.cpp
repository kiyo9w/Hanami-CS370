#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <map>
#include <iomanip> // For pretty printing JSON

#include "../common/token.h" // Needs TokenType, etc.
#include "../common/ast.h"   // Needs AST node definitions
#include "../common/json.hpp" // Needs JSON library
#include "../common/json_deserializer.h" // Include the shared deserializer

// --- Forward Declarations for Deserialization --- 
std::unique_ptr<ASTNode> fromJson(const nlohmann::json& j);
std::unique_ptr<Expression> expressionFromJson(const nlohmann::json& j);
std::unique_ptr<Statement> statementFromJson(const nlohmann::json& j);

// --- Enhanced Symbol Table --- 

enum class SymbolType { VARIABLE, FUNCTION, SPECIES, UNKNOWN };

enum class Visibility { PUBLIC = 2, PRIVATE = 3, PROTECTED = 4, DEFAULT = 0 };

struct SymbolEntry {
    std::string name;
    std::string typeName; // e.g., "int", "void", "Rose", or "int()" for functions before enhancement
    SymbolType kind = SymbolType::UNKNOWN;
    int scopeLevel = 0;
    Visibility visibility = Visibility::DEFAULT;
    std::string parentSpecies = ""; // Tên của species chứa member này (nếu là member)
    
    // For functions/methods: store parameter types
    std::vector<std::string> parameterTypes; 
    // Add more info: is_param, is_member, visibility, etc.
}; 

struct SpeciesMemberInfo {
    std::string name;
    std::string type;
    SymbolType kind;
    Visibility visibility;
};

class SymbolTable {
public:
    SymbolTable() { enterScope(); } // Start with global scope

    void enterScope() {
        scopes_.emplace_back();
        currentLevel_++;
    }

    void exitScope() {
        if (!scopes_.empty()) {
            scopes_.pop_back();
        }
        if(currentLevel_ > 0) currentLevel_--;
    }

    // Define a symbol in the current scope
    bool define(const std::string& name, const std::string& type, SymbolType kind, 
                Visibility visibility = Visibility::DEFAULT, const std::string& parentSpecies = "",
                const std::vector<std::string>& paramTypes = {}) {
        if (scopes_.empty()) return false; // Should not happen
        
        // Check if already defined in the *current* scope
        auto& currentScope = scopes_.back();
        // For non-members, check current scope. For members, check speciesMembers_
        bool alreadyDefined = false;
        if (!parentSpecies.empty()) {
            if (speciesMembers_.count(parentSpecies) && speciesMembers_[parentSpecies].count(name)) {
                alreadyDefined = true;
            }
        } else {
            if (currentScope.count(name)) {
                 alreadyDefined = true;
            }
        }

        if (alreadyDefined) {
            return false; // Already defined in this scope or species
        }
        
        SymbolEntry entry = {name, type, kind, currentLevel_};
        entry.visibility = visibility;
        entry.parentSpecies = parentSpecies;
        if (kind == SymbolType::FUNCTION) {
            entry.parameterTypes = paramTypes;
        }
        
        // Add to current scope (for local lookup within methods/blocks)
        currentScope[name] = entry; 
        
        // Nếu đây là một member của species, lưu vào speciesMembers_
        if (!parentSpecies.empty()) {
            // Check if species map exists, if not create it (should exist if SpeciesDecl was processed)
            speciesMembers_[parentSpecies][name] = entry; 
        }
        
        return true;
    }

    // Find a symbol by searching current and outer scopes
    SymbolEntry* lookup(const std::string& name) {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            auto& scope = *it;
            if (scope.count(name)) {
                return &scope[name];
            }
        }
        return nullptr; // Not found
    }
    
    // Lookup a member within a specific species context
    SymbolEntry* lookupMember(const std::string& memberName, const std::string& speciesName,
                              const std::string& analysisContext) {
        // Primarily look within the permanent species member storage
        auto speciesIt = speciesMembers_.find(speciesName);
        if (speciesIt != speciesMembers_.end()) {
            auto& members = speciesIt->second;
            auto memberIt = members.find(memberName);
            if (memberIt != members.end()) {
                // Found the member entry
                SymbolEntry* entry = &memberIt->second; // Get pointer to the stored entry

                // Check visibility
                if (entry->visibility == Visibility::PUBLIC || analysisContext == speciesName) {
                    return entry; // Found accessible member
                } else {
                    // Found but not accessible from this context
                    return nullptr;
                }
            }
        }
        // Member not found in the species definition
        return nullptr; 
    }
    
    int getCurrentLevel() const { return currentLevel_; }
    
    // Helper to check member existence and basic visibility 
    // (Used before calling lookupMember to get the full entry)
    bool hasAccessibleMember(const std::string& speciesName, const std::string& memberName, 
                             const std::string& analysisContext) {
        auto it = speciesMembers_.find(speciesName);
        if (it != speciesMembers_.end()) {
            const auto& members = it->second;
            auto memberIt = members.find(memberName);
            if (memberIt != members.end()) {
                const auto& memberEntry = memberIt->second;
                // Check visibility
                return memberEntry.visibility == Visibility::PUBLIC || analysisContext == speciesName;
            }
        }
        return false;
    }
    
    // Thiết lập context species hiện tại (dùng khi phân tích bên trong species)
    void setCurrentSpeciesContext(const std::string& speciesName) {
        currentSpeciesContext_ = speciesName;
    }
    
    // Get all member names for a species (could be useful for checks like unused members later)
    std::vector<std::string> getMemberNames(const std::string& speciesName) {
        auto it = speciesMembers_.find(speciesName);
        std::vector<std::string> names;
        if (it != speciesMembers_.end()) {
            for(const auto& pair : it->second) {
                names.push_back(pair.first);
            }
        }
        return names;
    }

private:
    std::vector<std::map<std::string, SymbolEntry>> scopes_;
    int currentLevel_ = -1;
    
    // Store full SymbolEntry for members of each species, keyed by species name, then member name.
    std::unordered_map<std::string, std::map<std::string, SymbolEntry>> speciesMembers_;
    
    // Species context hiện tại - dùng để kiểm tra quyền truy cập private/protected
    std::string currentSpeciesContext_ = "";
};

// --- Semantic Analysis Visitor --- 

class SemanticAnalyzerVisitor {
public:
    SemanticAnalyzerVisitor() = default;

    void analyze(ASTNode* node) {
        errors_.clear();
        currentSpeciesName_ = ""; // Reset context
        currentFunctionReturnType_ = "";
        visit(node);
    }

    bool hasErrors() const {
        return !errors_.empty();
    }

    void printErrors() const {
        for (const auto& err : errors_) {
            std::cerr << "Semantic Error: " << err << std::endl;
        }
    }

private:
    SymbolTable symbolTable_;
    std::vector<std::string> errors_;
    std::string currentFunctionReturnType_;
    std::string currentSpeciesName_; // Track the current species context

    // Helper to record errors
    void error(const std::string& message) {
        errors_.push_back(message);
    }
    
    // Helper to infer expression type (Enhanced)
    std::string typeOf(Expression* expr) {
        if (!expr) return "";

        // Basic Literals
        if (dynamic_cast<NumberLiteralExpr*>(expr)) return "int";
        if (dynamic_cast<StringLiteralExpr*>(expr)) return "string";
        if (dynamic_cast<BooleanLiteralExpr*>(expr)) return "bool";
        if (dynamic_cast<FloatLiteralExpr*>(expr)) return "float";
        if (dynamic_cast<DoubleLiteralExpr*>(expr)) return "double";

        // Variables/Functions/Species instances
        if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(expr)) {
            SymbolEntry* entry = symbolTable_.lookup(ident->name);

            if (!entry) {
                error("Undeclared identifier '" + ident->name + "' used in expression.");
                return "";
            }
            // If it's a function symbol, return its base return type (strip "()")
             if (entry->kind == SymbolType::FUNCTION) {
                 std::string returnType = entry->typeName;
                  if (returnType.length() >= 2 && returnType.substr(returnType.length() - 2) == "()") {
                      return returnType.substr(0, returnType.length() - 2);
                  }
                 error("Invalid function signature stored for '" + ident->name + "'. Found: " + returnType);
                 return ""; // Should not happen if defined correctly
             }
            return entry->typeName;
        }

        // Binary Operations
        if (BinaryOpExpr* binOp = dynamic_cast<BinaryOpExpr*>(expr)) {
            std::string leftType = typeOf(binOp->left.get());
            std::string rightType = typeOf(binOp->right.get());

            if (leftType.empty() || rightType.empty()) return ""; // Avoid cascading errors

            // Helper lambda to check if a type is numeric
            auto isNumeric = [](const std::string& type) {
                return type == "int" || type == "float" || type == "double";
            };

            // Arithmetic Operations
            if (binOp->op == TokenType::PLUS || binOp->op == TokenType::MINUS ||
                binOp->op == TokenType::STAR || binOp->op == TokenType::SLASH) {
                if (isNumeric(leftType) && isNumeric(rightType)) {
                    // Type promotion rules: double > float > int
                    if (leftType == "double" || rightType == "double") return "double";
                    if (leftType == "float" || rightType == "float") return "float";
                    return "int"; // Both must be int
                }
                // Allow string concatenation for PLUS
                if (binOp->op == TokenType::PLUS && leftType == "string" && rightType == "string") {
                     return "string";
                }
                 error("Arithmetic operation requires numeric types (int, float, double) or string concatenation, but got '" + leftType + "' and '" + rightType + "'.");
                 return "";
             }
            // Modulo (typically integer only)
            if (binOp->op == TokenType::MODULO) {
                 if (leftType == "int" && rightType == "int") return "int";
                 error("Modulo operation requires 'int' types, but got '" + leftType + "' and '" + rightType + "'.");
                 return "";
            }
            // Logical
            if (binOp->op == TokenType::AND || binOp->op == TokenType::OR) {
                if (leftType == "bool" && rightType == "bool") return "bool";
                error("Logical operation requires 'bool' types, but got '" + leftType + "' and '" + rightType + "'.");
                return "";
            }
            // Comparison
            if (binOp->op == TokenType::EQUAL || binOp->op == TokenType::NOT_EQUAL ||
                binOp->op == TokenType::LESS || binOp->op == TokenType::LESS_EQUAL ||
                binOp->op == TokenType::GREATER || binOp->op == TokenType::GREATER_EQUAL)
            {
                 // Allow comparison between any two numeric types
                 if (isNumeric(leftType) && isNumeric(rightType)) return "bool";
                 // Allow comparison between two strings
                 if (leftType == "string" && rightType == "string") return "bool";
                 // Allow comparison between two bools (for == and !=)
                 if ((binOp->op == TokenType::EQUAL || binOp->op == TokenType::NOT_EQUAL) && leftType == "bool" && rightType == "bool") return "bool";
                 
                 error("Comparison between incompatible types '" + leftType + "' and '" + rightType + "'.");
                 return "";
            }

            error("Unsupported binary operator '" + tokenTypeToString(binOp->op) + "' for types '" + leftType + "' and '" + rightType + "'.");
            return "";
        }

        // Function Calls - Phiên bản tổng quát
        if (FunctionCallExpr* call = dynamic_cast<FunctionCallExpr*>(expr)) {
            if (IdentifierExpr* calleeIdent = dynamic_cast<IdentifierExpr*>(call->callee.get())) {
                SymbolEntry* funcEntry = symbolTable_.lookup(calleeIdent->name);
                if (!funcEntry || funcEntry->kind != SymbolType::FUNCTION) {
                    error("Attempting to call undeclared or non-function identifier '" + calleeIdent->name + "'.");
                    return "";
                }
                
                // Check arguments
                const auto& expectedParams = funcEntry->parameterTypes;
                if (call->arguments.size() != expectedParams.size()) {
                    error("Function '" + calleeIdent->name + "' expects " + std::to_string(expectedParams.size()) + 
                          " arguments, but got " + std::to_string(call->arguments.size()) + ".");
                    return ""; // Return empty type on error
                }

                for (size_t i = 0; i < expectedParams.size(); ++i) {
                    std::string argType = typeOf(call->arguments[i].get());
                    if (!argType.empty() && argType != expectedParams[i]) {
                        error("Argument type mismatch in call to '" + calleeIdent->name + "'. Expected '" + 
                              expectedParams[i] + "' for argument " + std::to_string(i+1) + ", but got '" + argType + "'.");
                        // Continue checking other args, but overall call result is invalid
                    }
                }
                
                // Return the function's declared return type (now stored directly in typeName)
                return funcEntry->typeName;
            } 
            else if (MemberAccessExpr* memberCall = dynamic_cast<MemberAccessExpr*>(call->callee.get())) {
                std::string objectType = typeOf(memberCall->object.get());
                if (objectType.empty()) return ""; // Error already reported
                
                std::string methodName = memberCall->member->name;
 
                // --- Member Function Lookup & Argument Check ---
                SymbolEntry* methodEntry = symbolTable_.lookupMember(methodName, objectType, currentSpeciesName_);

                if (!methodEntry || methodEntry->kind != SymbolType::FUNCTION) {
                    error("Cannot find accessible member function '" + methodName + "' in species '" + objectType + "'.");
                    return "";
                }

                // Check arguments (similar to regular function call)
                const auto& expectedParams = methodEntry->parameterTypes;
                if (call->arguments.size() != expectedParams.size()) {
                    error("Method '" + methodName + "' expects " + std::to_string(expectedParams.size()) +
                          " arguments, but got " + std::to_string(call->arguments.size()) + ".");
                    return ""; // Return empty type on error
                }

                for (size_t i = 0; i < expectedParams.size(); ++i) {
                    std::string argType = typeOf(call->arguments[i].get());
                    if (!argType.empty() && argType != expectedParams[i]) {
                        error("Argument type mismatch in call to '" + methodName + "'. Expected '" +
                              expectedParams[i] + "' for argument " + std::to_string(i+1) + ", but got '" + argType + "'.");
                        // Continue checking other args
                    }
                }

                // Return the method's declared return type
                return methodEntry->typeName;
                // --- End Member Function Lookup ---
            } 
            else {
                error("Invalid callee type for function call.");
                return "";
            }
        }

        // Assignment (is also an expression)
         if (AssignmentStmt* assign = dynamic_cast<AssignmentStmt*>(expr)) {
             // Type of assignment is the type of the right-hand side
             std::string rightType = typeOf(assign->right.get());
             std::string leftType = "";

             // Check if left side is assignable (L-value)
             bool isLValue = false;
             // Check if left side is assignable (L-value) & type compatibility
             if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(assign->left.get())) {
                  SymbolEntry* entry = symbolTable_.lookup(ident->name);
                   if (!entry) {
                       error("Cannot assign to undeclared identifier '" + ident->name + "'.");
                       return "";
                   } else {
                       // TODO: Check if variable is const
                       isLValue = true; 
                       leftType = entry->typeName;
                   }
             } else if (MemberAccessExpr* member = dynamic_cast<MemberAccessExpr*>(assign->left.get())) {
                  std::string objectType = typeOf(member->object.get());
                  std::string memberName = member->member->name;

                  if (!objectType.empty()) { // Only check member if object type is known
                       SymbolEntry* speciesEntry = symbolTable_.lookup(objectType);
                       if (speciesEntry && speciesEntry->kind == SymbolType::SPECIES) {
                            // Use the proper SymbolTable method to lookup the member
                            SymbolEntry* memberEntry = symbolTable_.lookupMember(memberName, objectType, currentSpeciesName_);

                            if (memberEntry) {
                                // Check if assigning to a method
                                if (memberEntry->kind == SymbolType::FUNCTION) {
                                    error("Cannot assign to method '" + memberName + "'.");
                                    return ""; // Invalid assignment
                                }
                                // TODO: Check if member is const
                                isLValue = true;
                                leftType = memberEntry->typeName;
                            } else {
                                error("Cannot find accessible member variable '" + memberName + "' in species '" + objectType + "' for assignment.");
                                return "";
                            }
                       } else {
                             error("Cannot assign to member '" + memberName + "' of non-species type '" + objectType + "'.");
                            return "";
                       }
                     }
             }

             // Perform checks if LHS is valid and types are known
             if (!isLValue) {
                  error("Invalid left-hand side for assignment.");
                  return "";
              }
             
             // Check assignment compatibility
             bool compatible = false;
             if (leftType == rightType) {
                 compatible = true;
             } else {
                 // Allow assigning int to float/double, or float to double
                 if ((leftType == "float" || leftType == "double") && rightType == "int") compatible = true;
                 if (leftType == "double" && rightType == "float") compatible = true;
             }

             if (!leftType.empty() && !rightType.empty() && !compatible) {
                   error("Type mismatch: Cannot assign value of type '" + rightType + "' to L-value of type '" + leftType + "'.");
                   return ""; // Return empty on type error
             }
             
             return rightType; // Assignment expression evaluates to the assigned value's type (rhs)
         }


        // Member Access (e.g., g.member) - evaluation type
        if (MemberAccessExpr* memberAccess = dynamic_cast<MemberAccessExpr*>(expr)) {
            std::string objectType = typeOf(memberAccess->object.get());
            if (objectType.empty()) return ""; // Error already reported

            SymbolEntry* speciesEntry = symbolTable_.lookup(objectType);
             if (!speciesEntry || speciesEntry->kind != SymbolType::SPECIES) {
                 error("Cannot access member '" + memberAccess->member->name + "' on non-species type '" + objectType + "'.");
                 return "";
             }
             
             // Use the proper SymbolTable method to lookup the member
             std::string memberName = memberAccess->member->name;
             SymbolEntry* memberEntry = symbolTable_.lookupMember(memberName, objectType, currentSpeciesName_);

             if (memberEntry) {
                  // Check if accessing a function like a variable
                  if (memberEntry->kind == SymbolType::FUNCTION) {
                       error("Cannot access method '" + memberName + "' like a variable. Use () to call.");
                       return "";
                  }
                  // It's a variable, return its type
                  return memberEntry->typeName;
             } 

            // Member not found or not accessible
            error("Cannot find accessible member variable '" + memberName + "' in species '" + objectType + "'.");
            return "";
        }

        // ... Add other expression types ...

        error("Unable to determine type for this expression node.");
        return "";
    }

    // --- Visitor Methods --- 

    void visit(ASTNode* node) {
        if (!node) return;
        
        // Dispatch based on type
        if (auto* p = dynamic_cast<ProgramNode*>(node)) visitProgram(p);
        else if (auto* p = dynamic_cast<StyleIncludeStmt*>(node)) visitStyleInclude(p);
        else if (auto* p = dynamic_cast<GardenDeclStmt*>(node)) visitGardenDecl(p);
        else if (auto* p = dynamic_cast<SpeciesDeclStmt*>(node)) visitSpeciesDecl(p);
        else if (auto* p = dynamic_cast<VisibilityBlockStmt*>(node)) visitVisibilityBlock(p);
        else if (auto* p = dynamic_cast<BlockStmt*>(node)) visitBlock(p);
        else if (auto* p = dynamic_cast<VariableDeclStmt*>(node)) visitVariableDecl(p);
        else if (auto* p = dynamic_cast<FunctionDefStmt*>(node)) visitFunctionDef(p);
        else if (auto* p = dynamic_cast<ReturnStmt*>(node)) visitReturn(p);
        else if (auto* p = dynamic_cast<ExpressionStmt*>(node)) visitExpressionStmt(p);
        else if (auto* p = dynamic_cast<BranchStmt*>(node)) visitBranch(p);
        else if (auto* p = dynamic_cast<IOStmt*>(node)) visitIO(p);
        else if (dynamic_cast<Expression*>(node)) { 
            // Should not happen at statement level
        } else {
             error("Unsupported AST node type encountered during semantic analysis.");
        }
    }

    void visitProgram(ProgramNode* node) {
         symbolTable_.enterScope(); // Global Scope
         for (const auto& stmt : node->statements) {
             visit(stmt.get());
         }
          symbolTable_.exitScope();
    }

    void visitStyleInclude(StyleIncludeStmt* node) { /* Usually ignored */ }
    void visitGardenDecl(GardenDeclStmt* node) { /* Namespace handling if needed */ }

    void visitSpeciesDecl(SpeciesDeclStmt* node) {
        if (!symbolTable_.define(node->name, node->name, SymbolType::SPECIES)) {
            error("Species '" + node->name + "' already defined in this scope.");
        }
        
        std::string previousSpeciesName = currentSpeciesName_;
        currentSpeciesName_ = node->name; // Set current species context
        
        // Thiết lập context trong symbol table để kiểm tra quyền truy cập
        symbolTable_.setCurrentSpeciesContext(node->name);
        
        symbolTable_.enterScope(); // Enter species member scope
        
        // First pass: define members in the symbol table
        for (const auto& section : node->sections) {
            if (!section) continue;
            
            // Lấy visibility từ section
            Visibility visibility = Visibility::DEFAULT;
            if (auto* visBlock = dynamic_cast<VisibilityBlockStmt*>(section.get())) {
                visibility = static_cast<Visibility>(visBlock->visibility);
            }
            
            if (!section->block) continue;
            
            for (const auto& stmt : section->block->statements) {
                if (auto* varDecl = dynamic_cast<VariableDeclStmt*>(stmt.get())) {
                    if (!symbolTable_.define(varDecl->varName, varDecl->typeName, 
                                           SymbolType::VARIABLE, visibility, node->name)) {
                        error("Member variable '" + varDecl->varName + "' already declared in species '" + node->name + "'.");
                    }
                } else if (auto* funcDef = dynamic_cast<FunctionDefStmt*>(stmt.get())) {
                    // Lưu thông tin method với return type
                    // Collect parameter types
                    std::vector<std::string> paramTypes;
                    for (const auto& param : funcDef->parameters) {
                        paramTypes.push_back(param.typeName);
                    }
                    if (!symbolTable_.define(funcDef->name, funcDef->returnType, 
                                          SymbolType::FUNCTION, visibility, node->name, paramTypes)) {
                        error("Method '" + funcDef->name + "' already declared in species '" + node->name + "'.");
                    }
                }
            }
        }
        
        // Second pass: visit sections để phân tích method bodies
        for (const auto& section : node->sections) {
            visit(section.get());
        }
        
        symbolTable_.exitScope(); // Exit species scope
        
        // Khôi phục context trước đó
        symbolTable_.setCurrentSpeciesContext(previousSpeciesName);
        currentSpeciesName_ = previousSpeciesName;
    }

    void visitVisibilityBlock(VisibilityBlockStmt* node) {
        // Visit the statements within the block
        if (node->block) {
            // We don't create a new scope here, members belong to the species scope
            for (const auto& stmt : node->block->statements) {
                 visit(stmt.get());
            }
        }
    }

    void visitBlock(BlockStmt* node) {
        // Only create a new scope if NOT directly inside a species visibility block
        bool newScopeNeeded = (symbolTable_.getCurrentLevel() > 0 && currentSpeciesName_.empty()); // Crude check
         if(newScopeNeeded) symbolTable_.enterScope(); 
        for (const auto& stmt : node->statements) {
            visit(stmt.get());
        }
         if(newScopeNeeded) symbolTable_.exitScope();
    }

    void visitVariableDecl(VariableDeclStmt* node) {
         // If inside a species, this was handled in visitSpeciesDecl first pass
         if (!currentSpeciesName_.empty()) {
              // Analyze initializer if present
             if (node->initializer) {
                 std::string initializerType = typeOf(node->initializer.get());
                  if (!initializerType.empty() && initializerType != node->typeName) {
                       error("Type mismatch: Cannot initialize member variable '" + node->varName +
                             "' of type '" + node->typeName + "' with expression of type '" +
                             initializerType + "'.");
                  }
             }
             return; // Definition already handled
         }

         // --- Regular variable declaration ---
         // Check type exists
         if (node->typeName != "int" && node->typeName != "string" && node->typeName != "bool") {
             SymbolEntry* typeEntry = symbolTable_.lookup(node->typeName);
             if (!typeEntry || typeEntry->kind != SymbolType::SPECIES) { // Allow species types
                 error("Unknown type '" + node->typeName + "' for variable '" + node->varName + "'.");
             }
         }

        // Check initializer type
        if (node->initializer) {
             std::string initializerType = typeOf(node->initializer.get());
             if (!initializerType.empty() && initializerType != node->typeName) {
                  error("Type mismatch: Cannot initialize variable '" + node->varName +
                        "' of type '" + node->typeName + "' with expression of type '" +
                        initializerType + "'.");
             }
        }

        // Define the variable
        if (!symbolTable_.define(node->varName, node->typeName, SymbolType::VARIABLE)) {
            error("Variable '" + node->varName + "' already declared in this scope.");
        }
    }

    void visitFunctionDef(FunctionDefStmt* node) {
          // If inside a species, the symbol was defined in visitSpeciesDecl first pass.
         // We still need to analyze the body.

         // Collect parameter types
         std::vector<std::string> paramTypes;
         for (const auto& param : node->parameters) {
             paramTypes.push_back(param.typeName);
         }

         // Set context for return type checking
         std::string previousFunctionReturnType = currentFunctionReturnType_;
         currentFunctionReturnType_ = node->returnType;

         // Define the function symbol if not already defined (e.g., if not a method)
         // Store only return type in 'typeName', param types are separate
         if (currentSpeciesName_.empty()) { // Only define if not a method (methods defined in visitSpeciesDecl)
             if (!symbolTable_.define(node->name, node->returnType, SymbolType::FUNCTION, Visibility::DEFAULT, "", paramTypes)) {
                 error("Function '" + node->name + "' already defined in this scope.");
                 // Even if redefined, continue analysis of the body with the new definition's scope
             }
         }

         symbolTable_.enterScope(); // Enter function parameter/body scope
         // Define parameters
         for (const auto& param : node->parameters) {
              // Check parameter type validity
              if (param.typeName != "int" && param.typeName != "string" && param.typeName != "bool") {
                  if (!symbolTable_.lookup(param.typeName)) { // Allow species types
                      error("Unknown type '" + param.typeName + "' for parameter '" + param.paramName + "' in function '" + node->name + "'.");
                  }
              }
              // Define parameter in function scope
              if (!symbolTable_.define(param.paramName, param.typeName, SymbolType::VARIABLE)) {
                  error("Parameter '" + param.paramName + "' redeclared in function '" + node->name + "'.");
              }
         }

         // Visit function body
         if (node->body) {
              // For methods, body doesn't create another scope level directly via visitBlock
              for (const auto& stmt : node->body->statements) {
                   visit(stmt.get());
              }
         } else {
              error("Function '" + node->name + "' has no body.");
         }
         
         symbolTable_.exitScope(); // Exit function scope

         currentFunctionReturnType_ = previousFunctionReturnType; // Restore outer context
    }

    void visitReturn(ReturnStmt* node) {
        std::string returnExprType = "void";
        if (node->returnValue) {
            returnExprType = typeOf(node->returnValue.get());
            if (returnExprType.empty()) return; // Error already reported by typeOf
        }

        if (currentFunctionReturnType_.empty()) {
            error("'blossom' (return) statement found outside of a function definition.");
        } else if (returnExprType != currentFunctionReturnType_) {
             if (currentFunctionReturnType_ == "void" && node->returnValue) {
                  error("Cannot return a value from a 'void' function.");
             } else if (currentFunctionReturnType_ != "void" && !node->returnValue){
                  error("Must return a value of type '" + currentFunctionReturnType_ + "' from non-void function.");
             }
             else {
                 // Allow returning int literal 0 from int main even if declared different? No, stick to declared type.
                 error("Return type mismatch: Cannot return value of type '" + returnExprType +
                       "' from function expecting '" + currentFunctionReturnType_ + "'.");
            }
        }
    }

    void visitExpressionStmt(ExpressionStmt* node) {
        // Analyze the expression
        typeOf(node->expression.get());
    }

    void visitBranch(BranchStmt* node) {
        for (const auto& branch : node->branches) {
            if (branch.condition) {
                std::string conditionType = typeOf(branch.condition.get());
                if (!conditionType.empty() && conditionType != "bool") {
                    error("Condition for 'branch' must be of type bool, but got '" + conditionType + "'.");
                }
            }
             if(branch.body) { // Visit body block
                 visit(branch.body.get());
             }
        }
    }

    void visitIO(IOStmt* node) {
        for (const auto& expr : node->expressions) {
            std::string exprType = typeOf(expr.get());
             if (exprType.empty()) continue; // Error already reported

             if (node->direction == TokenType::STREAM_IN) {
                 if (!dynamic_cast<IdentifierExpr*>(expr.get()) && !dynamic_cast<MemberAccessExpr*>(expr.get())) { // Allow reading into members
                      error("'water >>' can only read into variables or assignable members.");
                 }
                 // Could also check if the variable/member is assignable (not const etc. if language had it)
             }
             // Could add checks for << on complex types if needed (e.g., require toString method)
        }
    }
};

int main(int argc, char* argv[]) {
    std::string inputFilename = "input/input.ast"; // Default input AST file
    std::string outputFilename = "output/output.ir"; // Default output IR file

    if (argc > 1) {
        inputFilename = std::string(argv[1]);
    }
    if (argc > 2) {
        outputFilename = std::string(argv[2]);
    }

    std::cout << "Semantic Analyzer Module" << std::endl;
    std::cout << "Reading AST from: " << inputFilename << std::endl;

    // Read JSON AST file
    std::ifstream inFile(inputFilename);
    if (!inFile) {
        std::cerr << "Error: Could not open input AST file: " << inputFilename << std::endl;
        return 1;
    }
    nlohmann::json astJson;
    try {
        inFile >> astJson;
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "Error: Failed to parse input AST JSON: " << e.what() << std::endl;
        return 1;
    }
    inFile.close();

    std::cout << "Deserializing AST..." << std::endl;
    std::unique_ptr<ASTNode> astRoot = nullptr;
    try {
         astRoot = fromJson(astJson); // Deserialize JSON to C++ AST objects
    } catch (const std::exception& e) {
         std::cerr << "Error during AST deserialization: " << e.what() << std::endl;
         return 1;
    }
    

    if (!astRoot) {
        std::cerr << "Error: Failed to deserialize AST from JSON." << std::endl;
        return 1;
    }
     // Check if the root is actually a ProgramNode
     ProgramNode* programRoot = dynamic_cast<ProgramNode*>(astRoot.get());
     if (!programRoot) {
          std::cerr << "Error: Deserialized AST root is not a ProgramNode." << std::endl;
          return 1;
     }

    std::cout << "Performing semantic analysis..." << std::endl;
    SemanticAnalyzerVisitor analyzer;
    analyzer.analyze(programRoot); 

    if (analyzer.hasErrors()) {
        std::cout << "Semantic analysis finished with errors:" << std::endl;
        analyzer.printErrors();
        // Decide if we should still generate IR despite errors
        // For now, let's stop.
         std::cerr << "IR file not generated due to semantic errors." << std::endl;
         return 1; 
    } else {
         std::cout << "Semantic analysis finished successfully." << std::endl;
    }
    
    std::cout << "Writing annotated AST/IR to: " << outputFilename << std::endl;

    // Re-serialize the (potentially annotated) AST to JSON for the IR file
    // For now, just re-serialize the original structure.
    // TODO: Modify toJson() methods in ast.h or create new IR nodes 
    //       and corresponding toJson() methods if the output format differs.
    nlohmann::json outputJson = astRoot->toJson(); 

    std::ofstream outFile(outputFilename);
    if (!outFile) {
        std::cerr << "Error: Could not open output IR file: " << outputFilename << std::endl;
        return 1;
    }

    outFile << std::setw(4) << outputJson << std::endl;
    outFile.close();

    std::cout << "IR successfully written to " << outputFilename << std::endl;

    return 0;
}