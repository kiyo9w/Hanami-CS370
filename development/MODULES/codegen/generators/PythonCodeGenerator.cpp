// Included by codegen/main.cpp

#include <string>
#include <sstream>
#include <stdexcept>
#include <set>

class PythonCodeGenerator : public CodeGeneratorVisitor {
public:
    std::string generate(ASTNode* node) override {
        generatedCode_.str("");
        generatedCode_.clear();
        indentLevel = 0;
        hasMain_ = false;
        mainFunctionName_ = ""; // Store the actual main function name found
        currentSpeciesName_ = ""; // Reset context
        currentFuncParams_.clear(); // Clear params
        
        // Add imports if needed (e.g., for specific functionality)
        // generatedCode_ << "import sys\n\n";
        
        dispatch(node); // Start visiting
        
        // Add main guard if a main function was found
        if (hasMain_) {
             generatedCode_ << "\n\nif __name__ == \"__main__\":\n";
             // Call the specific main function identified
             generatedCode_ << "    " << mainFunctionName_ << "()\n";
        }
        
        return generatedCode_.str();
    }

private:
    std::stringstream generatedCode_;
    bool hasMain_ = false;
    std::string mainFunctionName_ = ""; // Added
    std::string currentSpeciesName_; // Track if inside a class
    std::set<std::string> currentFuncParams_; // Added: Track current func params

    // --- Type Mapping (Python is dynamically typed, less critical) ---
    std::string mapType(const std::string& hanamiType) {
        // Python doesn't use type declarations in the same way,
        // but this is useful for generating type hints and comments
        if (hanamiType == "int") return "int";
        if (hanamiType == "bool") return "bool";
        if (hanamiType == "string") return "str";
        if (hanamiType == "void") return "None";
        if (hanamiType == "float") return "float";
        if (hanamiType == "double") return "float";  // Python uses a single float type
        return hanamiType; // Assume species name is Python class
    }
    
    // --- Operator Mapping ---
    std::string mapBinaryOperator(TokenType op) {
         switch(op) {
            case TokenType::PLUS: return "+";
            case TokenType::MINUS: return "-";
            case TokenType::STAR: return "*";
            case TokenType::SLASH: return "/"; // Python 3 float division
            case TokenType::MODULO: return "%";
            case TokenType::EQUAL: return "==";
            case TokenType::NOT_EQUAL: return "!=";
            case TokenType::LESS: return "<";
            case TokenType::LESS_EQUAL: return "<=";
            case TokenType::GREATER: return ">";
            case TokenType::GREATER_EQUAL: return ">=";
            case TokenType::AND: return "and";
            case TokenType::OR: return "or";
            default: return "#?#";
         }
    }
    
    // --- Visitor Implementations ---
    std::string visitProgram(ProgramNode* node) override {
        for (const auto& stmt : node->statements) {
            generatedCode_ << dispatch(stmt.get());
        }
        return ""; // Handled by generate()
    }

    std::string visitStyleInclude(StyleIncludeStmt* node) override { return ""; /* Ignore for Python */ }
    std::string visitGardenDecl(GardenDeclStmt* node) override { return ""; /* No direct equivalent */ }

    std::string visitSpeciesDecl(SpeciesDeclStmt* node) override {
        std::string speciesCode;
        speciesCode += getIndent() + "class " + node->name + ":\n";
        indentLevel++;
        
        std::string previousSpeciesName = currentSpeciesName_;
        currentSpeciesName_ = node->name; // Set context
        
        std::string constructorCode = "";
        std::string methodsCode = "";
        bool members_exist = false;

        // Collect members and methods, generate constructor for initialization
        for (const auto& section : node->sections) {
            if(!section->block) continue;
            for(const auto& stmt : section->block->statements) {
                 if(auto* varDecl = dynamic_cast<VariableDeclStmt*>(stmt.get())){
                     // Generate initialization in constructor
                      constructorCode += getIndent() + "  self." + varDecl->varName + " = ";
                      if(varDecl->initializer){
                           constructorCode += dispatchExpr(varDecl->initializer.get()) + "\n";
                      } else {
                           // Map basic types to Python defaults
                           if(varDecl->typeName == "int") constructorCode += "0\n";
                           else if(varDecl->typeName == "string") constructorCode += "\"\"\n"; 
                           else if(varDecl->typeName == "bool") constructorCode += "False\n";
                           else constructorCode += "None\n"; // Default for unknown/species types
                      }
                     members_exist = true;
                 } else if (auto* funcDef = dynamic_cast<FunctionDefStmt*>(stmt.get())){
                     // Generate method
                     methodsCode += dispatch(funcDef); // visitFunctionDef adds 'self'
                     members_exist = true;
                 }
            }
        }

        // Assemble class: constructor first, then methods
        if (!constructorCode.empty()) {
             speciesCode += getIndent() + "def __init__(self):\n";
             speciesCode += constructorCode;
             speciesCode += "\n"; // Blank line after constructor
        }

        speciesCode += methodsCode;

        // Add a pass statement if the class is otherwise empty
         if (!members_exist && constructorCode.empty() && methodsCode.empty()) {
             speciesCode += getIndent() + "pass\n";
         }
         
        indentLevel--;
        speciesCode += "\n"; // Add a blank line after class def
        
        currentSpeciesName_ = previousSpeciesName; // Restore context
        return speciesCode;
    }

    std::string visitVisibilityBlock(VisibilityBlockStmt* node) override {
         // Python visibility is handled by convention/name mangling
         // The contents are processed by visitSpeciesDecl
         return ""; // Don't generate anything for the block itself
    }

    std::string visitBlock(BlockStmt* node) override {
        std::string blockCode = "";
        for (const auto& stmt : node->statements) {
             blockCode += dispatch(stmt.get());
        }
         // Add pass if block is empty (relevant in functions, if/else etc)
         if (blockCode.empty()) {
             // The caller (e.g., visitFunctionDef) needs to know if it was empty
             // Let's return empty and let caller add pass.
             // blockCode = getIndent() + "pass\n";
         }
        return blockCode;
    }

    std::string visitVariableDecl(VariableDeclStmt* node) override {
        // Python is dynamically typed, so type name is ignored for declaration.
        std::string code = getIndent() + node->varName;
        if (node->initializer) {
            code += " = " + dispatchExpr(node->initializer.get());
        } else {
             // Initialize based on scope and type name convention
             // Check if it's likely a class instantiation (capitalized type name)
             if (!currentSpeciesName_.empty()) { // Inside a class -> handled by __init__
                  // This shouldn't be called for member variables directly,
                  // as they are initialized in __init__.
                  // If called, it might be a local variable inside a method.
                 code += " = None";
             } else if (!node->typeName.empty() && std::isupper(node->typeName[0])){
                 // Likely a class type in global/main scope
                 code += " = " + mapType(node->typeName) + "()"; // Instantiate class
             } else {
                  code += " = None"; // Default for other types or non-class locals
             }
        }
        code += "\n";
        return code;
    }

    std::string visitFunctionDef(FunctionDefStmt* node) override {
         bool isMethod = !currentSpeciesName_.empty();
         currentFuncParams_.clear(); // Clear params from previous function
         
         // Handle main specially
         if (!isMethod && (node->name == "mainGarden" || node->name == "main")) {
            hasMain_ = true;
            mainFunctionName_ = node->name; // Store the exact name used
         }
         
         std::string code = getIndent() + "def " + node->name + "(";
         if (isMethod) {
             code += "self"; // Add self for methods
             if (!node->parameters.empty()) code += ", ";
         }
         
         for (size_t i = 0; i < node->parameters.size(); ++i) {
            code += node->parameters[i].paramName;
            currentFuncParams_.insert(node->parameters[i].paramName); // Store param name
            if (i < node->parameters.size() - 1) code += ", ";
         }
         code += "):\n";
         indentLevel++;
         std::string bodyCode;
         if (node->body) {
             bodyCode = visitBlock(node->body.get());
         }
         if(bodyCode.empty()){ // Add pass if body is empty
             code += getIndent() + "pass\n";
         } else {
             code += bodyCode;
         }
         indentLevel--;
         code += "\n"; // Add a blank line after function def
         
         currentFuncParams_.clear(); // Clear params after visiting function
         return code;
    }

    std::string visitReturn(ReturnStmt* node) override {
         std::string code = getIndent() + "return";
         if (node->returnValue) {
             code += " " + dispatchExpr(node->returnValue.get());
         }
         code += "\n";
         return code;
    }

    std::string visitExpressionStmt(ExpressionStmt* node) override {
         // Assignments are expressions in Python
         return getIndent() + dispatchExpr(node->expression.get()) + "\n";
    }

    std::string visitBranch(BranchStmt* node) override {
         std::string code = "";
         bool first = true;
         for(const auto& branch : node->branches) {
             std::string bodyCode = "";
              if (branch.body) {
                   // Temporarily increase indent for the block content generation
                   indentLevel++; 
                   bodyCode = visitBlock(branch.body.get());
                   indentLevel--;
              }
              if (bodyCode.empty()) { // Python requires something in a block
                   indentLevel++;
                   bodyCode = getIndent() + "pass\n"; 
                   indentLevel--;
              }
              
             code += getIndent(); // Indent the if/elif/else keyword
             if (first) {
                 code += "if " + dispatchExpr(branch.condition.get()) + ":\n";
                 first = false;
             } else {
                 if (branch.condition) { // elif
                      code += "elif " + dispatchExpr(branch.condition.get()) + ":\n";
                 } else { // else
                      code += "else:\n";
                 }
             }
              // Add the body code, already indented correctly relative to the block
              code += bodyCode;
         }
         return code;
    }

    std::string visitIO(IOStmt* node) override {
         std::string code = "";
          if (node->ioType == TokenType::BLOOM) { // Output
              code += getIndent() + "print(";
               std::vector<std::string> exprStrings;
               for (const auto& expr : node->expressions) {
                   exprStrings.push_back(dispatchExpr(expr.get()));
               }
               // Join expressions. Convert non-strings if needed.
               // Simple concatenation for now, might need str()
                for (size_t i = 0; i < exprStrings.size(); ++i) {
                     code += exprStrings[i]; // Assume they are printable
                     if (i < exprStrings.size() - 1) code += ", "; // Print with spaces
                }
               code += ", end='')\n"; // Avoid default newline from print
          } else if (node->ioType == TokenType::WATER) { // Input
               for (const auto& expr : node->expressions) {
                    // Check if assigning to a member variable
                    std::string targetVar = "";
                    if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(expr.get())) {
                        if (!currentSpeciesName_.empty() && 
                            currentFuncParams_.find(ident->name) == currentFuncParams_.end())
                        {
                             targetVar = "self." + ident->name;
                        } else {
                             targetVar = ident->name;
                        }
                         code += getIndent() + targetVar + " = input() # TODO: Add type conversion if needed (e.g., int(), float())\n";
                    } else { 
                        code += getIndent() + "# Error: Cannot read input into non-variable\n";
                    }
               }
          }
          return code;
    }
    
    // --- Expression Visitors ---
     std::string visitIdentifierExpr(IdentifierExpr* node) override {
         // Prepend "self." if inside a class method and the identifier
         // is not a parameter of the current function.
         if (!currentSpeciesName_.empty() && 
             currentFuncParams_.find(node->name) == currentFuncParams_.end()) 
         {
              // Simple check: Assume it's a member if not a parameter
              // This is still a heuristic - needs symbol table info for accuracy
             // Let's apply it for now
             return "self." + node->name;
         }
          return node->name; 
     }
     std::string visitNumberLiteralExpr(NumberLiteralExpr* node) override { return node->value.empty() ? "0" : node->value; }
     std::string visitFloatLiteralExpr(FloatLiteralExpr* node) override {
         // Python handles floats directly
         return node->value;
     }
     std::string visitDoubleLiteralExpr(DoubleLiteralExpr* node) override {
         // Python treats floats and doubles similarly (as float)
         return node->value;
     }
     std::string visitStringLiteralExpr(StringLiteralExpr* node) override { 
         // Python uses quotes, escapes might need translation
          std::string escapedValue = "";
          for (char c : node->value) {
              switch (c) {
                  case '\\': escapedValue += "\\\\"; break;
                  case '"': escapedValue += "\\\""; break;
                  case '\n': escapedValue += "\\n"; break;
                  // Add other escapes if needed (e.g., \t, \r)
                  default:   escapedValue += c; break;
              }
          }
          return "\"" + escapedValue + "\""; 
     }
     std::string visitBooleanLiteralExpr(BooleanLiteralExpr* node) override { return node->value ? "True" : "False"; } // Capitalized
     
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
         return dispatchExpr(node->object.get()) + "." + node->member->name;
     }
     
      std::string visitAssignmentStmt(AssignmentStmt* node) override {
          std::string leftCode;
          // Check if the left side is an identifier that needs `self.`
          if(IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(node->left.get())) {
              if (!currentSpeciesName_.empty() && 
                  currentFuncParams_.find(ident->name) == currentFuncParams_.end()) 
              {
                  leftCode = "self." + ident->name;
              } else {
                  leftCode = ident->name;
              }
          } else {
              // Handle other L-values like member access (a.b = ...) if needed
              leftCode = dispatchExpr(node->left.get()); 
          }
          return leftCode + " = " + dispatchExpr(node->right.get());
      }

    std::string visitWhileStmt(WhileStmt* node) override {
        std::string code = getIndent() + "while ";
        if (node->condition) code += dispatchExpr(node->condition.get());
        code += ":\n";
        indentLevel++;
        std::string bodyCode = "";
        if (node->body) bodyCode = visitBlock(node->body.get());
        if (bodyCode.empty()) { // Python requires a body
            code += getIndent() + "pass\n";
        } else {
            code += bodyCode;
        }
        indentLevel--;
        return code;
    }

    std::string visitForStmt(ForStmt* node) override {
        // Python's for loop is different (for item in iterable)
        // Translate standard C-style for loop to a while loop
        std::string code = "";
        // 1. Initializer
        if (node->initializer) {
            code += dispatch(node->initializer.get());
        }
        // 2. While condition
        code += getIndent() + "while (";
        if (node->condition) code += dispatchExpr(node->condition.get());
        else code += "True"; // Loop forever if no condition
        code += "):\n";
        indentLevel++;
        // 3. Body
        std::string bodyCode = "";
        if (node->body) bodyCode = visitBlock(node->body.get());
        // 4. Increment (append at the end of the body)
        if (node->increment) {
            bodyCode += getIndent() + dispatchExpr(node->increment.get()) + "\n";
        }

        if (bodyCode.empty()) {
            code += getIndent() + "pass\n";
        } else {
            code += bodyCode;
        }
        indentLevel--;
        return code;
    }

    // Dispatch expression nodes (return value as string)
    std::string dispatchExpr(Expression* expr) {
        // This is a simplified dispatch for expression nodes. 
        // It assumes expressions can be directly converted to string representations.
        if (!expr) return "None"; // Default Python representation for null/void
        
        // Using dynamic_cast to determine the type and call the specific visitor
        if (auto* p = dynamic_cast<IdentifierExpr*>(expr)) return visitIdentifierExpr(p);
        if (auto* p = dynamic_cast<NumberLiteralExpr*>(expr)) return visitNumberLiteralExpr(p);
        if (auto* p = dynamic_cast<FloatLiteralExpr*>(expr)) return visitFloatLiteralExpr(p);
        if (auto* p = dynamic_cast<DoubleLiteralExpr*>(expr)) return visitDoubleLiteralExpr(p);
        if (auto* p = dynamic_cast<StringLiteralExpr*>(expr)) return visitStringLiteralExpr(p);
        if (auto* p = dynamic_cast<BooleanLiteralExpr*>(expr)) return visitBooleanLiteralExpr(p);
        if (auto* p = dynamic_cast<BinaryOpExpr*>(expr)) return visitBinaryOpExpr(p);
        if (auto* p = dynamic_cast<FunctionCallExpr*>(expr)) return visitFunctionCallExpr(p);
        if (auto* p = dynamic_cast<MemberAccessExpr*>(expr)) return visitMemberAccessExpr(p);
        if (auto* p = dynamic_cast<AssignmentStmt*>(expr)) return visitAssignmentStmt(p);

        return "#<Unknown Expr>#"; // Placeholder for unhandled expression types
    }

}; 