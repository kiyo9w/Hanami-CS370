// Included by codegen/main.cpp

#include <string>
#include <sstream>
#include <stdexcept>
#include <set> // For tracking includes

class CppCodeGenerator : public CodeGeneratorVisitor {
public:
    std::string generate(ASTNode* node) override {
        generatedCode_.str("");
        generatedCode_.clear();
        includes_.clear();
        indentLevel = 0;
        hasMain_ = false;
        
        // Default includes
        includes_.insert("#include <iostream>");
        includes_.insert("#include <string>");
        includes_.insert("#include <vector>"); // Basic includes
        
        // Visit the AST to determine required headers & generate code structure
        std::string bodyCode = dispatch(node);
        
        // Write includes first
        for (const auto& include : includes_) {
            generatedCode_ << include << "\n";
        }
        generatedCode_ << "\n";
        
        // Write the generated body code
        generatedCode_ << bodyCode;
        
        // Add a default main if none was found
        if (!hasMain_) {
            generatedCode_ << "\nint main() {\n";
            generatedCode_ << "    std::cout << \"Hanami program (no main/grow function found)\" << std::endl;\n";
            generatedCode_ << "    return 0;\n";
            generatedCode_ << "}\n";
        }
        
        return generatedCode_.str();
    }

private:
    std::stringstream generatedCode_;
    std::set<std::string> includes_;
    bool hasMain_ = false;

    // --- Type Mapping --- 
    std::string mapType(const std::string& hanamiType) {
        if (hanamiType == "int") return "int";
        if (hanamiType == "bool") return "bool";
        if (hanamiType == "string") {
             includes_.insert("#include <string>"); // Ensure string is included
             return "std::string";
        }
        if (hanamiType == "void") return "void";
        // TODO: Map Species names (structs/classes)
        return hanamiType; // Assume species name is C++ class/struct name
    }
    
    // --- Operator Mapping ---
    std::string mapBinaryOperator(TokenType op) {
         switch(op) {
            case TokenType::PLUS: return "+";
            case TokenType::MINUS: return "-";
            case TokenType::STAR: return "*";
            case TokenType::SLASH: return "/";
            case TokenType::MODULO: return "%";
            case TokenType::EQUAL: return "==";
            case TokenType::NOT_EQUAL: return "!=";
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
        std::string code = "";
        for (const auto& stmt : node->statements) {
            code += dispatch(stmt.get());
        }
        return code; 
    }

    std::string visitStyleInclude(StyleIncludeStmt* node) override { 
        // Try to map to C++ includes
        // Needs careful handling of quotes and path vs library
        std::string path = node->path;
        // Remove potential outer quotes added by lexer/parser
        if (path.length() >= 2 && path.front() == '"' && path.back() == '"') {
            path = path.substr(1, path.length() - 2);
        }
        // Basic check for standard library headers
        if (path.find('.') == std::string::npos) { // Assume standard header if no dot
            includes_.insert("#include <" + path + ">");
        } else { // Assume local header
            includes_.insert("#include \"" + path + "\"");
        }
        return ""; // Include is added at the top level
    }
    
    std::string visitGardenDecl(GardenDeclStmt* node) override { 
        return "// Garden: " + node->name + " (namespace omitted for simplicity)\n"; 
    }

    std::string visitSpeciesDecl(SpeciesDeclStmt* node) override {
        std::string speciesCode;
        speciesCode += getIndent() + "struct " + node->name + " {\n"; // Use struct for simplicity
        indentLevel++;
        // Proper C++ would require separating declarations and definitions,
        // and handling public/protected/private explicitly.
        // This simplified version puts everything inside.
        for (const auto& section : node->sections) {
            speciesCode += dispatch(section.get());
        }
        indentLevel--;
        speciesCode += getIndent() + "};\n\n"; // Add semicolon after struct def
        return speciesCode;
    }

    std::string visitVisibilityBlock(VisibilityBlockStmt* node) override {
        // Add C++ access specifiers
        std::string code = "";
        switch(node->visibility) {
            case TokenType::OPEN: code += getIndent() + "public:\n"; break;
            case TokenType::GUARDED: code += getIndent() + "protected:\n"; break; 
            case TokenType::HIDDEN: code += getIndent() + "private:\n"; break;
            default: break;
        }
        
        // Add indent temporarily for members under specifier
        // Note: This assumes block only contains declarations
        indentLevel++; 
        code += dispatch(node->block.get());
        indentLevel--;
        return code;
    }

    std::string visitBlock(BlockStmt* node) override {
        std::string blockCode = "";
        // C++ needs braces for blocks
        // blockCode += getIndent() + "{\n"; 
        // indentLevel++;
        for (const auto& stmt : node->statements) {
             blockCode += dispatch(stmt.get());
        }
        // indentLevel--;
        // blockCode += getIndent() + "}\n";
        // Braces are typically handled by the caller (FunctionDef, Branch)
        return blockCode;
    }

    std::string visitVariableDecl(VariableDeclStmt* node) override {
        std::string code = getIndent();
        code += mapType(node->typeName) + " " + node->varName;
        if (node->initializer) {
            code += " = " + dispatchExpr(node->initializer.get());
        }
        code += ";\n";
        return code;
    }

    std::string visitFunctionDef(FunctionDefStmt* node) override {
         std::string code = getIndent();
          bool isMain = (node->name == "mainGarden" || node->name == "main");
          if (isMain) {
              code += "int main"; // C++ main returns int
              hasMain_ = true;
          } else {
              code += mapType(node->returnType) + " " + node->name;
          }
          
         code += "(";
         for (size_t i = 0; i < node->parameters.size(); ++i) {
            code += mapType(node->parameters[i].typeName) + " " + node->parameters[i].paramName;
            if (i < node->parameters.size() - 1) code += ", ";
         }
         code += ") {\n";
         indentLevel++;
         if (node->body) {
             // Generate body statements
             code += visitBlock(node->body.get()); 
         }
          // Add default return 0 for int main()
          if (isMain && node->returnType != "int") { // Check if a blossom 0; was missing
             bool hasReturn = false;
              if(node->body){
                 for(const auto& stmt : node->body->statements){
                     if(dynamic_cast<ReturnStmt*>(stmt.get())) { hasReturn = true; break; }
                 }
              }
             if (!hasReturn) code += getIndent() + "return 0;\n";
          }
         indentLevel--;
         code += getIndent() + "}\n\n";
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
         includes_.insert("#include <iostream>"); // Ensure iostream
         std::string code = "";
         std::string stream = (node->ioType == TokenType::BLOOM) ? "std::cout" : "std::cin";
         std::string op = (node->direction == TokenType::STREAM_OUT) ? " << " : " >> ";
         
         code += getIndent() + stream;
         for (const auto& expr : node->expressions) {
             code += op + dispatchExpr(expr.get());
         }
         code += ";\n";
         if(node->ioType == TokenType::BLOOM) {
             // If the original Hanami code implicitly added endl, add it here.
             // Assuming simple stream for now.
         }
         return code;
    }
    
    // --- Expression Visitors ---
     std::string visitIdentifierExpr(IdentifierExpr* node) override { 
         // Handle std::string
          if(node->name == "std::string") return "std::string";
         return node->name; 
     }
     std::string visitNumberLiteralExpr(NumberLiteralExpr* node) override { return node->value.empty() ? "0" : node->value; }
     std::string visitStringLiteralExpr(StringLiteralExpr* node) override { 
          includes_.insert("#include <string>");
          return "std::string(\"" + node->value + "\")"; // Construct std::string
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
         // Use . for objects/structs, -> for pointers (assume objects for now)
         return dispatchExpr(node->object.get()) + "." + node->member->name;
     }
     
      std::string visitAssignmentStmt(AssignmentStmt* node) override {
          return dispatchExpr(node->left.get()) + " = " + dispatchExpr(node->right.get());
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
            // Need to handle potential semicolon from VariableDecl
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