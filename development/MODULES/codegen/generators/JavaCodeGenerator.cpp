// Included by codegen/main.cpp

#include <string>
#include <sstream>
#include <stdexcept>

class JavaCodeGenerator : public CodeGeneratorVisitor {
public:
    std::string generate(ASTNode* node) override {
        // Reset state for new generation if needed
        generatedCode_.str(""); 
        generatedCode_.clear();
        std::string gardenName = ""; // Find garden name
        hasMain_ = false;
        indentLevel = 0;
        
        // Find Garden name
        if(auto* p = dynamic_cast<ProgramNode*>(node)){
             for(const auto& stmt : p->statements){
                 if(auto* g = dynamic_cast<GardenDeclStmt*>(stmt.get())){
                     gardenName = g->name;
                     break;
                 }
             }
        }
        
        // Determine class name: Use default, non-public if garden exists
        className_ = gardenName.empty() ? "GeneratedHanamiClass" : "HanamiProgram"; 

        generatedCode_ << "import java.util.Scanner;\n\n";
        generatedCode_ << "public class " << className_ << " {\n"; // Use the determined class name
        indentLevel++;

        // Add a static Scanner instance for input
        generatedCode_ << getIndent() << "private static Scanner inputScanner = new Scanner(System.in);\n\n";

        dispatch(node); // Start visiting
        
         // Add a default main if none was generated from AST
         if (!hasMain_) {
            generatedCode_ << getIndent() << "public static void main(String[] args) {\n";
            indentLevel++;
            generatedCode_ << getIndent() << "System.out.println(\"Hanami program (no main/grow function found)\");\n";
            indentLevel--;
            generatedCode_ << getIndent() << "}\n";
         }
         
        indentLevel--;
        generatedCode_ << "}\n";
        return generatedCode_.str();
    }

private:
    std::stringstream generatedCode_;
    std::string className_ = "GeneratedHanamiClass";
    bool hasMain_ = false;
    
    // --- Type Mapping --- 
    std::string mapType(const std::string& hanamiType) {
        if (hanamiType == "int") return "int";
        if (hanamiType == "bool") return "boolean";
        if (hanamiType == "string") return "String";
        if (hanamiType == "void") return "void";
        // TODO: Map Species names to class names
        return hanamiType; // Assume species name is Java class name
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
        for (const auto& stmt : node->statements) {
            generatedCode_ << dispatch(stmt.get());
        }
        return ""; // Handled by generate()
    }

    std::string visitStyleInclude(StyleIncludeStmt* node) override { return ""; /* Ignore for Java */ }
    std::string visitGardenDecl(GardenDeclStmt* node) override { return ""; /* Handled in generate() */ }

    std::string visitSpeciesDecl(SpeciesDeclStmt* node) override {
        // Generate a class for the species
        std::string speciesCode;
        speciesCode += getIndent() + "public static class " + node->name + " {\n"; // Static inner class for simplicity
        indentLevel++;
        for (const auto& section : node->sections) {
            speciesCode += dispatch(section.get());
        }
        // TODO: Add constructor?
        indentLevel--;
        speciesCode += getIndent() + "}\n\n";
        return speciesCode;
    }

    std::string visitVisibilityBlock(VisibilityBlockStmt* node) override {
         // Map Hanami visibility to Java access modifiers
         std::string modifier = "";
         switch(node->visibility) {
             case TokenType::OPEN: modifier = "public "; break;
             case TokenType::GUARDED: modifier = "protected "; break; // Or package-private? 
             case TokenType::HIDDEN: modifier = "private "; break;
             default: break;
         }
         // Apply modifier to contained declarations (more complex in reality)
          std::string blockCode = dispatch(node->block.get()); // Visit block first
          // This simple approach won't work perfectly for modifiers; 
          // Modifiers need to be applied *during* member/method visit
          // For now, just return the block code.
          return blockCode;
          // We need to pass the modifier down to visitVariableDecl/visitFunctionDef
    }

    std::string visitBlock(BlockStmt* node) override {
        std::string blockCode = "";
        // Note: Java doesn't usually need standalone braces unless for specific scope needs
        // Here, we just process the statements.
        for (const auto& stmt : node->statements) {
             blockCode += dispatch(stmt.get());
        }
        return blockCode;
    }

    std::string visitVariableDecl(VariableDeclStmt* node) override {
        std::string code = getIndent();
        // TODO: Need to handle visibility modifier passed down
        std::string javaType = mapType(node->typeName);
        code += javaType + " " + node->varName;
        if (node->initializer) {
            code += " = " + dispatchExpr(node->initializer.get());
        } else {
            // Initialize object types (simple check: starts with uppercase)
            if (!node->typeName.empty() && std::isupper(node->typeName[0])) {
                 // Assuming default constructor exists
                 code += " = new " + javaType + "()"; 
            }
            // Primitives like int, boolean are auto-initialized to defaults (0, false) in Java fields,
            // but local variables need explicit initialization if read before written.
            // For simplicity, we only explicitly initialize assumed objects here.
        }
        code += ";\n";
        return code;
    }

    std::string visitFunctionDef(FunctionDefStmt* node) override {
         std::string code = getIndent();
         // TODO: Handle visibility modifier
         bool isMain = (node->name == "mainGarden" || node->name == "main"); // Approximation
         if(isMain) {
             code += "public static ";
             hasMain_ = true;
         } else {
              code += "public "; // Default to public methods for now
         }
         
         code += mapType(node->returnType) + " " + node->name + "(";
         for (size_t i = 0; i < node->parameters.size(); ++i) {
            code += mapType(node->parameters[i].typeName) + " " + node->parameters[i].paramName;
            if (i < node->parameters.size() - 1) code += ", ";
         }
         code += ") {\n";
         indentLevel++;
         // Function body is a BlockStmt, visit its *statements*
         if (node->body) {
            for(const auto& stmt : node->body->statements){
                 code += dispatch(stmt.get());
            }
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
         // Handle assignment specifically if it wasn't handled in dispatchExpr
         if(dynamic_cast<AssignmentStmt*>(node->expression.get())){
              return getIndent() + dispatchExpr(node->expression.get()) + ";\n";
         }
          // Other expressions used as statements (e.g., function calls)
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
                      // Body block generation
                       if (branch.body) {
                            for(const auto& stmt : branch.body->statements){
                                 code += dispatch(stmt.get());
                            }
                       }
                      indentLevel--;
                      code += getIndent() + "}\n";
                      continue; // Skip rest of loop for else
                 }
             }
              if (branch.condition) code += dispatchExpr(branch.condition.get()) + ") ";
              code += "{\n";
              indentLevel++;
              // Body block generation
               if (branch.body) {
                   for(const auto& stmt : branch.body->statements){
                        code += dispatch(stmt.get());
                   }
              }
              indentLevel--;
              code += getIndent() + "}\n";
         }
         return code;
    }

    std::string visitIO(IOStmt* node) override {
         std::string code = "";
          if (node->ioType == TokenType::BLOOM) { // Output
              code += getIndent() + "System.out.print(";
               for (size_t i = 0; i < node->expressions.size(); ++i) {
                  code += dispatchExpr(node->expressions[i].get());
                  if (i < node->expressions.size() - 1) code += " + "; // Concatenate for print
               }
               code += ");\n"; // Use print, handle println via \n in string literal
          } else if (node->ioType == TokenType::WATER) { // Input
               // Use the static scanner - reads as String, manual conversion may be needed
               for (const auto& expr : node->expressions) {
                    if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(expr.get())) {
                        code += getIndent() + ident->name + " = inputScanner.nextLine(); // Reads as String\n";
                        // TODO: Manual type conversion needed here if target is not String (e.g., Integer.parseInt)
                    } else { 
                        code += getIndent() + "/* Error: Cannot read input into non-variable */\n";
                    }
               }
          }
          return code;
    }
    
    // --- Expression Visitors ---
     std::string visitIdentifierExpr(IdentifierExpr* node) override { return node->name; }
     std::string visitNumberLiteralExpr(NumberLiteralExpr* node) override { return node->value.empty() ? "0" : node->value; }
     std::string visitStringLiteralExpr(StringLiteralExpr* node) override { 
         // Java uses double quotes, escapes might need translation?
          return "\"" + node->value + "\""; 
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
         // Simple dot notation for Java
         return dispatchExpr(node->object.get()) + "." + node->member->name;
     }
     
      std::string visitAssignmentStmt(AssignmentStmt* node) override {
          return dispatchExpr(node->left.get()) + " = " + dispatchExpr(node->right.get());
      }

}; 