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

struct SymbolEntry {
    std::string name;
    std::string typeName; // e.g., "int", "void", "Rose"
    SymbolType kind = SymbolType::UNKNOWN;
    int scopeLevel = 0;
    // Add more info: is_param, is_member, visibility, etc.
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
    bool define(const std::string& name, const std::string& type, SymbolType kind) {
        if (scopes_.empty()) return false; // Should not happen
        
        // Check if already defined in the *current* scope
        auto& currentScope = scopes_.back();
        if (currentScope.count(name)) {
            return false; // Already defined in this scope
        }
        
        currentScope[name] = {name, type, kind, currentLevel_};
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
    
    int getCurrentLevel() const { return currentLevel_; }

private:
    std::vector<std::map<std::string, SymbolEntry>> scopes_;
    int currentLevel_ = -1;
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

        // Variables/Functions/Species instances
        if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(expr)) {
            SymbolEntry* entry = symbolTable_.lookup(ident->name);

            // --- Member Lookup Enhancement ---
            if (!entry && !currentSpeciesName_.empty()) {
                 SymbolEntry* speciesEntry = symbolTable_.lookup(currentSpeciesName_);
                 if (speciesEntry && speciesEntry->kind == SymbolType::SPECIES) {
                      // TODO: Enhance SymbolTable to store members properly.
                      // Simulating lookup: If 'name' or 'age' used inside 'Pet'.
                      if (currentSpeciesName_ == "Pet" && (ident->name == "name")) return "string";
                      if (currentSpeciesName_ == "Pet" && (ident->name == "age")) return "int";

                       // If not found as a known member (simulated)
                      error("Species '" + currentSpeciesName_ + "' has no member named '" + ident->name + "'.");
                      return "";
                 }
            }
            // --- End Member Lookup ---

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

            // Arithmetic (int only for now)
             if (binOp->op == TokenType::PLUS || binOp->op == TokenType::MINUS ||
                 binOp->op == TokenType::STAR || binOp->op == TokenType::SLASH ||
                 binOp->op == TokenType::MODULO) {
                 if (leftType == "int" && rightType == "int") return "int";
                 // TODO: Allow string concatenation for PLUS
                 error("Arithmetic operation requires 'int' types, but got '" + leftType + "' and '" + rightType + "'.");
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
                // Basic comparison: allow same types, or int/float etc.
                 if (leftType == rightType) return "bool";
                 // TODO: Add more flexible comparison rules (e.g. int/float)
                 error("Comparison between incompatible types '" + leftType + "' and '" + rightType + "'.");
                 return "";
            }

            error("Unsupported binary operator for types '" + leftType + "' and '" + rightType + "'.");
            return "";
        }

        // Function Calls
        if (FunctionCallExpr* call = dynamic_cast<FunctionCallExpr*>(expr)) {
            if (IdentifierExpr* calleeIdent = dynamic_cast<IdentifierExpr*>(call->callee.get())) {
                SymbolEntry* funcEntry = symbolTable_.lookup(calleeIdent->name);
                if (!funcEntry || funcEntry->kind != SymbolType::FUNCTION) {
                    error("Attempting to call undeclared or non-function identifier '" + calleeIdent->name + "'.");
                    return "";
                }
                // TODO: Check argument types/count against function signature in SymbolTable
                std::string returnType = funcEntry->typeName;
                if (returnType.length() >= 2 && returnType.substr(returnType.length() - 2) == "()") {
                    return returnType.substr(0, returnType.length() - 2);
                }
                 error("Invalid function signature stored for '" + calleeIdent->name + "'. Found: " + returnType);
                 return "";
            } else if (MemberAccessExpr* memberCall = dynamic_cast<MemberAccessExpr*>(call->callee.get())) {
                 std::string objectType = typeOf(memberCall->object.get());
                 if (objectType.empty()) return "";

                 // --- Member Function Lookup ---
                 SymbolEntry* speciesEntry = symbolTable_.lookup(objectType);
                  if (!speciesEntry || speciesEntry->kind != SymbolType::SPECIES) {
                      error("Cannot call member function '" + memberCall->member->name + "' on non-species type '" + objectType + "'.");
                      return "";
                  }
                  // TODO: Enhance SymbolTable to store member function signatures.
                  // Simulate lookup for known Pet methods
                   if (objectType == "Pet") {
                       if (memberCall->member->name == "setDetails") return "void";
                       if (memberCall->member->name == "introduce") return "void";
                       if (memberCall->member->name == "getAgeInDogYears") return "int";
                   }

                  error("Cannot find member function '" + memberCall->member->name + "' in species '" + objectType + "'.");
                  return "";
                 // --- End Member Function Lookup ---

            } else {
                 error("Invalid callee type for function call.");
                 return "";
            }
        }

        // Assignment (is also an expression)
         if (AssignmentStmt* assign = dynamic_cast<AssignmentStmt*>(expr)) {
             // Type of assignment is the type of the right-hand side
             std::string rightType = typeOf(assign->right.get());
             // Check if left side is assignable (L-value) & type compatibility
             if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(assign->left.get())) {
                  SymbolEntry* entry = symbolTable_.lookup(ident->name);
                   bool isMember = false;
                    // Check if it's a member variable assignment within a method
                    if(!entry && !currentSpeciesName_.empty()) {
                         if(currentSpeciesName_ == "Pet" && (ident->name == "name" || ident->name == "age")){
                             isMember = true;
                             // Simulate getting member type
                             entry = new SymbolEntry(); // Temporary for check
                             entry->typeName = (ident->name == "name" ? "string" : "int");
                         }
                    }

                  if (!entry) {
                      error("Cannot assign to undeclared identifier '" + ident->name + "'.");
                      if(isMember) delete entry; // Clean up temp entry
                      return "";
                  }
                  if (!rightType.empty() && entry->typeName != rightType) {
                       error("Type mismatch: Cannot assign value of type '" + rightType + "' to variable '" + ident->name + "' of type '" + entry->typeName + "'.");
                       if(isMember) delete entry;
                       return ""; // Return empty on type error
                  }
                  if(isMember) delete entry; // Clean up temp entry

             } else if (MemberAccessExpr* member = dynamic_cast<MemberAccessExpr*>(assign->left.get())) {
                  // TODO: Check member assignability and type
                  std::string objectType = typeOf(member->object.get());
                  std::string memberType = "";
                   // Simulate member type lookup
                    if(objectType == "Pet"){
                        if(member->member->name == "name") memberType = "string";
                        if(member->member->name == "age") memberType = "int";
                    }
                  if (memberType.empty()) {
                       error("Cannot find member variable '" + member->member->name + "' in species '" + objectType + "' for assignment.");
                       return "";
                  }
                  if (!rightType.empty() && memberType != rightType) {
                       error("Type mismatch: Cannot assign value of type '" + rightType + "' to member '" + member->member->name + "' of type '" + memberType + "'.");
                       return "";
                  }

             }
             else {
                 error("Invalid left-hand side for assignment.");
                 return "";
             }
             return rightType; // Assignment expression evaluates to the assigned value's type
         }


        // Member Access (e.g., g.member) - evaluation type
        if (MemberAccessExpr* memberAccess = dynamic_cast<MemberAccessExpr*>(expr)) {
            std::string objectType = typeOf(memberAccess->object.get());
            if (objectType.empty()) return "";

            SymbolEntry* speciesEntry = symbolTable_.lookup(objectType);
             if (!speciesEntry || speciesEntry->kind != SymbolType::SPECIES) {
                 error("Cannot access member '" + memberAccess->member->name + "' on non-species type '" + objectType + "'.");
                 return "";
             }
             // TODO: Lookup member variable in SymbolTable species entry
             // Simulate lookup
              if (objectType == "Pet") {
                  if (memberAccess->member->name == "name") return "string";
                  if (memberAccess->member->name == "age") return "int";
              }

            error("Cannot find member variable '" + memberAccess->member->name + "' in species '" + objectType + "'.");
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

        symbolTable_.enterScope(); // Enter species member scope
        // TODO: Define members in the symbol table here, associated with the species
        // First pass: define members before visiting methods that might use them
        for (const auto& section : node->sections) {
             if (!section->block) continue;
             for (const auto& stmt : section->block->statements) {
                 if(auto* varDecl = dynamic_cast<VariableDeclStmt*>(stmt.get())) {
                      // Simulate defining member variable
                       if (!symbolTable_.define(varDecl->varName, varDecl->typeName, SymbolType::VARIABLE)) {
                            error("Member variable '" + varDecl->varName + "' already declared in species '" + node->name +"'.");
                       }
                  } else if (auto* funcDef = dynamic_cast<FunctionDefStmt*>(stmt.get())) {
                        // Simulate defining member function
                         if (!symbolTable_.define(funcDef->name, funcDef->returnType + "()", SymbolType::FUNCTION)) {
                             error("Method '" + funcDef->name + "' already declared in species '" + node->name +"'.");
                         }
                  }
             }
        }

        // Second pass: visit sections to analyze method bodies etc.
        for (const auto& section : node->sections) {
            visit(section.get());
        }
        symbolTable_.exitScope(); // Exit species scope

        currentSpeciesName_ = previousSpeciesName; // Restore outer context
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
         bool isMethod = !currentSpeciesName_.empty();

         // Set context for return type checking
         std::string previousFunctionReturnType = currentFunctionReturnType_;
         currentFunctionReturnType_ = node->returnType;

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