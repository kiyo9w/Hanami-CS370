// Included by codegen/main.cpp

#include <string>
#include <sstream>
#include <stdexcept>
#include <set>

class JavaScriptCodeGenerator : public CodeGeneratorVisitor {
public:
    std::string generate(ASTNode* node) override {
        generatedCode_.str("");
        generatedCode_.clear();
        indentLevel = 0;
        currentSpeciesName_ = ""; // Reset species context
        // JS doesn't usually have explicit main, but we might wrap in a function
        generatedCode_ << "// Generated Hanami Code (JavaScript)\n\n";
        dispatch(node); // Start visiting
        return generatedCode_.str();
    }

private:
    std::stringstream generatedCode_;
    std::string currentSpeciesName_; // Track if inside a class
    std::set<std::string> currentFuncParams_; // Added: Track current func params

    // --- Type Mapping (JS is dynamically typed) ---
    std::string mapType(const std::string& hanamiType) {
        // JavaScript is dynamically typed, but useful for comments
        if (hanamiType == "int") return "number";
        if (hanamiType == "bool") return "boolean";
        if (hanamiType == "string") return "string";
        if (hanamiType == "void") return "void";
        if (hanamiType == "float") return "number";
        if (hanamiType == "double") return "number";
        return hanamiType; // Assume species name is JS class
    }
    
    // --- Operator Mapping ---
    std::string mapBinaryOperator(TokenType op) {
         switch(op) {
            case TokenType::PLUS: return "+";
            case TokenType::MINUS: return "-";
            case TokenType::STAR: return "*";
            case TokenType::SLASH: return "/";
            case TokenType::MODULO: return "%";
            case TokenType::EQUAL: return "==="; // Use strict equality
            case TokenType::NOT_EQUAL: return "!=="; // Use strict inequality
            case TokenType::LESS: return "<";
            case TokenType::LESS_EQUAL: return "<=";
            case TokenType::GREATER: return ">";
            case TokenType::GREATER_EQUAL: return ">=";
            case TokenType::AND: return "&&";
            case TokenType::OR: return "||";
            default: return "/*?*/";
         }
    }
    
    // --- Visitor Implementations ---
    std::string visitProgram(ProgramNode* node) override {
        for (const auto& stmt : node->statements) {
            generatedCode_ << dispatch(stmt.get());
        }
        return ""; // Handled by generate()
    }

    std::string visitStyleInclude(StyleIncludeStmt* node) override { return ""; /* Ignore for JS */ }
    std::string visitGardenDecl(GardenDeclStmt* node) override { return "// Garden: " + node->name + "\n"; }

    std::string visitSpeciesDecl(SpeciesDeclStmt* node) override {
        std::string speciesCode;
        speciesCode += getIndent() + "class " + node->name + " {\n";
        indentLevel++;
        
        std::string previousSpeciesName = currentSpeciesName_;
        currentSpeciesName_ = node->name; // Set species context
        
        // Generate constructor to initialize members
        std::string constructorCode = "";
        std::string membersCode = "";
        std::string methodsCode = "";
        
        for (const auto& section : node->sections) {
             if(!section->block) continue;
             for (const auto& stmt : section->block->statements){
                 if(auto* varDecl = dynamic_cast<VariableDeclStmt*>(stmt.get())){
                     // Add member variable declaration (JS doesn't need type)
                     membersCode += getIndent() + varDecl->varName;
                      if(varDecl->initializer){
                         // Add initialization to constructor
                         constructorCode += getIndent() + "  this." + varDecl->varName + " = " + dispatchExpr(varDecl->initializer.get()) + ";\n";
                         membersCode += "; // Initialized in constructor\n";
                      } else {
                         // Add default initialization (e.g., null/undefined) to constructor
                         constructorCode += getIndent() + "  this." + varDecl->varName + " = null;\n"; 
                         membersCode += "; // Initialized in constructor\n";
                      }
                 } else if (auto* funcDef = dynamic_cast<FunctionDefStmt*>(stmt.get())){
                     // Add method definition
                     methodsCode += dispatch(funcDef); // visitFunctionDef handles method syntax
                 }
             }
        }
        
        // Assemble the class
        if (!constructorCode.empty()) {
             speciesCode += getIndent() + "constructor() {\n";
             speciesCode += constructorCode;
             speciesCode += getIndent() + "}\n\n";
        }
        
        // speciesCode += membersCode; // JS members are usually just initialized in constructor
        speciesCode += methodsCode;
        
        indentLevel--;
        speciesCode += getIndent() + "}\n\n";
        
        currentSpeciesName_ = previousSpeciesName; // Restore context
        
        return speciesCode;
    }

    std::string visitVisibilityBlock(VisibilityBlockStmt* node) override {
         // JS visibility is handled differently (or ignored for simplicity)
         // The content (members/methods) are processed by visitSpeciesDecl
         return ""; // Don't output anything for the block itself
    }

    std::string visitBlock(BlockStmt* node) override {
        std::string blockCode = "";
        // Braces handled by caller (function, if/else)
        for (const auto& stmt : node->statements) {
             blockCode += dispatch(stmt.get());
        }
        return blockCode;
    }

    std::string visitVariableDecl(VariableDeclStmt* node) override {
        std::string code = getIndent();
        // Use let/const? Defaulting to let.
        code += "let " + node->varName;
         if (node->initializer) {
             code += " = " + dispatchExpr(node->initializer.get());
         } else {
            // Initialize based on type name convention
            if (!currentSpeciesName_.empty()) { // Inside class -> handled by constructor
                code += "; // Initialized in constructor"; 
            } else if (!node->typeName.empty() && std::isupper(node->typeName[0])){
                code += " = new " + mapType(node->typeName) + "()"; // Instantiate class
            } else {
                 code += " = undefined"; // Default for others
            }
         }
         code += ";\n";
         return code;
    }

    std::string visitFunctionDef(FunctionDefStmt* node) override {
         bool isMethod = !currentSpeciesName_.empty();
         currentFuncParams_.clear(); // Clear params from previous function
         
         std::string code = getIndent();
         // Method syntax in JS: just name(params) { body }
         if (isMethod) {
              code += node->name + "(";
         } else { // Standalone function syntax
              code += "function " + node->name + "(";
         }
         
         for (size_t i = 0; i < node->parameters.size(); ++i) {
            code += node->parameters[i].paramName;
            currentFuncParams_.insert(node->parameters[i].paramName); // Store param name
            if (i < node->parameters.size() - 1) code += ", ";
         }
         code += ") {\n";
         indentLevel++;
         if (node->body) {
             code += visitBlock(node->body.get()); 
         }
         indentLevel--;
         code += getIndent() + "}\n\n";
         
         currentFuncParams_.clear(); // Clear params after visiting function
         return code;
    }

    std::string visitReturn(ReturnStmt* node) override {
         std::string code = getIndent() + "return";
         if (node->returnValue) {
             code += " " + dispatchExpr(node->returnValue.get());
         }
         code += ";\n";
         return code;
    }

    std::string visitExpressionStmt(ExpressionStmt* node) override {
         return getIndent() + dispatchExpr(node->expression.get()) + ";\n";
    }

    std::string visitBranch(BranchStmt* node) override {
         std::string code = "";
         bool first = true;
         for(const auto& branch : node->branches) {
             code += getIndent();
             if (first) {
                 code += "if (";
                 first = false;
             } else {
                 if (branch.condition) { // else if
                      code += "else if (";
                 } else { // else
                      code += "else {\n";
                      indentLevel++;
                       if (branch.body) code += visitBlock(branch.body.get());
                      indentLevel--;
                      code += getIndent() + "}\n";
                      continue; 
                 }
             }
              if (branch.condition) code += dispatchExpr(branch.condition.get()) + ") ";
              code += "{\n";
              indentLevel++;
              if (branch.body) code += visitBlock(branch.body.get());
              indentLevel--;
              code += getIndent() + "}\n";
         }
         return code;
    }

    std::string visitIO(IOStmt* node) override {
         std::string code = "";
         if (node->ioType == TokenType::BLOOM) { // Output
              code += getIndent() + "console.log(";
               std::vector<std::string> exprStrings;
               for (const auto& expr : node->expressions) {
                   exprStrings.push_back(dispatchExpr(expr.get()));
               }
                for (size_t i = 0; i < exprStrings.size(); ++i) {
                     code += exprStrings[i];
                     if (i < exprStrings.size() - 1) code += " + "; // String concat
                }
               code += ");\n"; 
          } else if (node->ioType == TokenType::WATER) { // Input
                // Basic browser input using prompt
                // Node.js would need require('readline')
                code += getIndent() + "// Basic input using prompt:\n";
                 for (const auto& expr : node->expressions) {
                    if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(expr.get())) {
                         // Assign directly, assuming var declared elsewhere
                         code += getIndent() + ident->name + " = parseFloat(prompt()); // Reads string, parses to float\n"; 
                         // TODO: Add parsing based on expected type? parseInt, parseFloat?
                    } else if (MemberAccessExpr* member = dynamic_cast<MemberAccessExpr*>(expr.get())) {
                        // Similar logic for member access - reads as string for now
                        std::string objectName = dispatchExpr(member->object.get()); 
                        std::string memberName = member->member->name;
                        code += getIndent() + objectName + "." + memberName + " = parseFloat(prompt()); // Reads string, parses to float\n"; 
                    } else { 
                        code += getIndent() + "/* Error: Cannot read input into non-variable */\n";
                    }
               }
          }
          return code;
    }
    
    // --- Expression Visitors ---
     std::string visitIdentifierExpr(IdentifierExpr* node) override { 
        // Prepend "this." if inside a class method and not a parameter
        if (!currentSpeciesName_.empty() && 
            currentFuncParams_.find(node->name) == currentFuncParams_.end()) 
        {
            return "this." + node->name;
        }
        return node->name; 
    }
     std::string visitNumberLiteralExpr(NumberLiteralExpr* node) override { return node->value.empty() ? "0" : node->value; }
     
     std::string visitFloatLiteralExpr(FloatLiteralExpr* node) override {
         // JavaScript uses the same number type for int/float/double
         return node->value;
     }
     
     std::string visitDoubleLiteralExpr(DoubleLiteralExpr* node) override {
         // JavaScript uses the same number type for int/float/double
         return node->value;
     }
     
     std::string visitStringLiteralExpr(StringLiteralExpr* node) override { 
         // JS uses quotes, consider template literals?
          std::string escapedValue = "";
          for (char c : node->value) {
              switch (c) {
                  case '\\': escapedValue += "\\\\"; break;
                  case '"': escapedValue += "\\\""; break;
                  case '\n': escapedValue += "\\n"; break;
                  // Add other escapes if needed (e.g., \t, \r, ')
                  default:   escapedValue += c; break;
              }
          }
          return "\"" + escapedValue + "\""; 
     }
     std::string visitBooleanLiteralExpr(BooleanLiteralExpr* node) override { return node->value ? "true" : "false"; }
     
     std::string visitBinaryOpExpr(BinaryOpExpr* node) override {
          return "(" + dispatchExpr(node->left.get()) + " " + 
                     mapBinaryOperator(node->op) + " " + 
                     dispatchExpr(node->right.get()) + ")";
     }
     
     std::string visitFunctionCallExpr(FunctionCallExpr* node) override {
         std::string code = dispatchExpr(node->callee.get()) + "(";
          for (size_t i = 0; i < node->arguments.size(); ++i) {
             code += dispatchExpr(node->arguments[i].get());
             if (i < node->arguments.size() - 1) code += ", ";
         }
         code += ")";
         return code;
     }
     
     std::string visitMemberAccessExpr(MemberAccessExpr* node) override {
         // Standard dot notation
         return dispatchExpr(node->object.get()) + "." + node->member->name;
     }
     
      std::string visitAssignmentStmt(AssignmentStmt* node) override {
          // Need to handle var/let/const declaration if LHS is just identifier?
          // Assuming variable already declared for assignment expressions.
          std::string leftCode;
          // Check if the left side is an identifier that needs `this.`
          if(IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(node->left.get())) {
              if (!currentSpeciesName_.empty() && 
                  currentFuncParams_.find(ident->name) == currentFuncParams_.end()) 
              {
                  leftCode = "this." + ident->name;
              } else {
                  leftCode = ident->name;
              }
          } else {
              // Handle other L-values like member access 
              leftCode = dispatchExpr(node->left.get()); 
          }
          return leftCode + " = " + dispatchExpr(node->right.get());
      }

    std::string visitWhileStmt(WhileStmt* node) override {
        std::string code = getIndent() + "while (";
        if (node->condition) code += dispatchExpr(node->condition.get());
        code += ") {\n";
        indentLevel++;
        if (node->body) code += visitBlock(node->body.get());
        indentLevel--;
        code += getIndent() + "}\n";
        return code;
    }

    std::string visitForStmt(ForStmt* node) override {
        std::string code = getIndent() + "for (";
        if (node->initializer) {
            std::string initStr = dispatch(node->initializer.get());
            // Remove trailing newline and potentially semicolon
            initStr.erase(initStr.find_last_not_of(" \n\r\t;") + 1);
            initStr.erase(0, initStr.find_first_not_of(" \n\r\t"));
            code += initStr + "; ";
        } else {
            code += "; ";
        }
        if (node->condition) code += dispatchExpr(node->condition.get());
        code += "; ";
        if (node->increment) code += dispatchExpr(node->increment.get());
        code += ") {\n";
        indentLevel++;
        if (node->body) code += visitBlock(node->body.get());
        indentLevel--;
        code += getIndent() + "}\n";
        return code;
    }

}; 