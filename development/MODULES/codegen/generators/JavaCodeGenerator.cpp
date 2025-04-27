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
        
        // Determine class name: Use garden name directly
        className_ = gardenName.empty() ? "GeneratedHanamiClass" : gardenName;

        // Add imports
        generatedCode_ << "// Converting Hanami code to Java\n";
        generatedCode_ << "import java.util.Scanner;\n\n";
        
        // Create the class declaration
        generatedCode_ << "public class " << className_ << " {\n";
        indentLevel++;

        // Add a static Scanner instance for input
        generatedCode_ << getIndent() << "private static Scanner inputScanner = new Scanner(System.in);\n\n";
        
        // Process all AST nodes
        dispatch(node);
        
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
    int indentLevel = 0;
    
    // Helper method to get proper indentation
    std::string getIndent() {
        return std::string(indentLevel * 4, ' ');
    }
    
    // --- Type Mapping --- 
    std::string mapType(const std::string& hanamiType) {
        if (hanamiType == "int") return "int";
        if (hanamiType == "bool") return "boolean";
        if (hanamiType == "string") return "String";
        if (hanamiType == "void") return "void";
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

    std::string visitStyleInclude(StyleIncludeStmt* node) override { 
        return ""; // Ignore includes in Java
    }
    
    std::string visitGardenDecl(GardenDeclStmt* node) override { 
        return ""; // Already handled in generate()
    }

    std::string visitSpeciesDecl(SpeciesDeclStmt* node) override {
        // Generate a class for the species
        std::string speciesCode;
        speciesCode += getIndent() + "static class " + node->name + " {\n";
        indentLevel++;
        
        // Track the current visibility for member declarations
        TokenType currentVisibility = TokenType::HIDDEN; // Default to private
        
        for (const auto& section : node->sections) {
            if (auto* visBlock = dynamic_cast<VisibilityBlockStmt*>(section.get())) {
                currentVisibility = visBlock->visibility;
                for (const auto& stmt : visBlock->block->statements) {
                    // Apply visibility modifier to each statement in the block
                    if (auto* varDecl = dynamic_cast<VariableDeclStmt*>(stmt.get())) {
                        speciesCode += getIndent();
                        switch (currentVisibility) {
                            case TokenType::OPEN: speciesCode += "public "; break;
                            case TokenType::GUARDED: speciesCode += "protected "; break;
                            case TokenType::HIDDEN: speciesCode += "private "; break;
                            default: break;
                        }
                        speciesCode += mapType(varDecl->typeName) + " " + varDecl->varName;
                        if (varDecl->initializer) {
                            speciesCode += " = " + dispatchExpr(varDecl->initializer.get());
                        }
                        speciesCode += ";\n";
                    }
                    else if (auto* funcDef = dynamic_cast<FunctionDefStmt*>(stmt.get())) {
                        speciesCode += getIndent();
                        switch (currentVisibility) {
                            case TokenType::OPEN: speciesCode += "public "; break;
                            case TokenType::GUARDED: speciesCode += "protected "; break;
                            case TokenType::HIDDEN: speciesCode += "private "; break;
                            default: break;
                        }
                        speciesCode += mapType(funcDef->returnType) + " " + funcDef->name + "(";
                        // Add parameters
                        for (size_t i = 0; i < funcDef->parameters.size(); ++i) {
                            speciesCode += mapType(funcDef->parameters[i].typeName) + " " + 
                                          funcDef->parameters[i].paramName;
                            if (i < funcDef->parameters.size() - 1) speciesCode += ", ";
                        }
                        speciesCode += ") {\n";
                        indentLevel++;
                        // Add function body
                        if (funcDef->body) {
                           for(const auto& bodyStmt : funcDef->body->statements){
                                speciesCode += dispatch(bodyStmt.get());
                           }
                        }
                        indentLevel--;
                        speciesCode += getIndent() + "}\n\n";
                    }
                    else {
                        speciesCode += dispatch(stmt.get());
                    }
                }
            }
            else {
                speciesCode += dispatch(section.get());
            }
        }
        
        indentLevel--;
        speciesCode += getIndent() + "}\n\n";
        return speciesCode;
    }

    std::string visitVisibilityBlock(VisibilityBlockStmt* node) override {
        // This is now handled in visitSpeciesDecl for better visibility control
        return "";
    }

    std::string visitBlock(BlockStmt* node) override {
        std::string blockCode = "";
        for (const auto& stmt : node->statements) {
             blockCode += dispatch(stmt.get());
        }
        return blockCode;
    }

    std::string visitVariableDecl(VariableDeclStmt* node) override {
        std::string code = getIndent();
        std::string javaType = mapType(node->typeName);
        code += javaType + " " + node->varName;
        
        if (node->initializer) {
            code += " = " + dispatchExpr(node->initializer.get());
        } else if (!node->typeName.empty() && std::isupper(node->typeName[0])) {
            // Initialize object types
            code += " = new " + javaType + "()";
        }
        
        code += ";\n";
        return code;
    }

    std::string visitFunctionDef(FunctionDefStmt* node) override {
        std::string code = getIndent();
        
        // IMPORTANT FIX: Check specifically for mainGarden function name
        // This is the entry point for Hanami programs
        if (node->name == "mainGarden") {
            hasMain_ = true;
            code += "public static void main(String[] args) {\n";
            indentLevel++;
            
            // Add function body with return value handling if needed
            if (node->body) {
                for (const auto& stmt : node->body->statements) {
                    // For return statements in mainGarden, convert to System.exit()
                    if (auto* returnStmt = dynamic_cast<ReturnStmt*>(stmt.get())) {
                        if (returnStmt->returnValue) {
                            code += getIndent() + "System.exit(" + dispatchExpr(returnStmt->returnValue.get()) + ");\n";
                        } else {
                            code += getIndent() + "System.exit(0);\n";
                        }
                    } else {
                        code += dispatch(stmt.get());
                    }
                }
            }
            
            indentLevel--;
            code += getIndent() + "}\n\n";
            return code;
        }
        // Handle normal function
        else {
            code += "public static " + mapType(node->returnType) + " " + node->name + "(";
            // Add parameters
            for (size_t i = 0; i < node->parameters.size(); ++i) {
                code += mapType(node->parameters[i].typeName) + " " + node->parameters[i].paramName;
                if (i < node->parameters.size() - 1) code += ", ";
            }
            code += ") {\n";
            
            indentLevel++;
            
            // Add function body
            if (node->body) {
                for(const auto& stmt : node->body->statements){
                    code += dispatch(stmt.get());
                }
            }
            
            indentLevel--;
            code += getIndent() + "}\n\n";
            return code;
        }
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
                if (branch.condition) {
                    code += "else if (";
                } else {
                    code += "else {\n";
                    indentLevel++;
                    
                    if (branch.body) {
                        for(const auto& stmt : branch.body->statements){
                            code += dispatch(stmt.get());
                        }
                    }
                    
                    indentLevel--;
                    code += getIndent() + "}\n";
                    continue;
                }
            }
            
            if (branch.condition) {
                code += dispatchExpr(branch.condition.get()) + ") {\n";
            }
            
            indentLevel++;
            
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
        std::string code = getIndent();
        
        if (node->ioType == TokenType::BLOOM) { // Output
            code += "System.out.print(";
            for (size_t i = 0; i < node->expressions.size(); ++i) {
                code += dispatchExpr(node->expressions[i].get());
                if (i < node->expressions.size() - 1) code += " + "; // Concatenate for print
            }
            code += ");\n"; // Use print, handle println via \n in string literal
        } else if (node->ioType == TokenType::WATER) { // Input
            // Use the static scanner - reads as String, manual conversion may be needed
            for (const auto& expr : node->expressions) {
                if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(expr.get())) {
                    code += ident->name + " = inputScanner.nextLine(); // Reads as String\n";
                    // TODO: Manual type conversion needed here if target is not String (e.g., Integer.parseInt)
                }
                else { 
                    code += "/* Error: Cannot read input into non-variable */\n";
                }
            }
        }
        
        return code;
    }
    
    // Expression dispatch helper
    std::string dispatchExpr(Expression* expr) {
        if (auto* ident = dynamic_cast<IdentifierExpr*>(expr)) {
            return visitIdentifierExpr(ident);
        }
        else if (auto* num = dynamic_cast<NumberLiteralExpr*>(expr)) {
            return visitNumberLiteralExpr(num);
        }
        else if (auto* str = dynamic_cast<StringLiteralExpr*>(expr)) {
            return visitStringLiteralExpr(str);
        }
        else if (auto* boolean = dynamic_cast<BooleanLiteralExpr*>(expr)) {
            return visitBooleanLiteralExpr(boolean);
        }
        else if (auto* binary = dynamic_cast<BinaryOpExpr*>(expr)) {
            return visitBinaryOpExpr(binary);
        }
        else if (auto* call = dynamic_cast<FunctionCallExpr*>(expr)) {
            return visitFunctionCallExpr(call);
        }
        else if (auto* access = dynamic_cast<MemberAccessExpr*>(expr)) {
            return visitMemberAccessExpr(access);
        }
        else if (auto* assign = dynamic_cast<AssignmentStmt*>(expr)) {
            return visitAssignmentStmt(assign);
        }
        return "/* Unknown expression */";
    }
    
    // --- Expression Visitors ---
    std::string visitIdentifierExpr(IdentifierExpr* node) override { 
        return node->name; 
    }
    
    std::string visitNumberLiteralExpr(NumberLiteralExpr* node) override { 
        // Java defaults to int
        return node->value;
    }
    
    std::string visitFloatLiteralExpr(FloatLiteralExpr* node) override {
        // Append 'f' for float literals in Java
        return node->value + "f";
    }
    
    std::string visitDoubleLiteralExpr(DoubleLiteralExpr* node) override {
        // Double literals are standard in Java
        return node->value;
    }
    
    std::string visitStringLiteralExpr(StringLiteralExpr* node) override { 
        return "\"" + node->value + "\""; 
    }
    
    std::string visitBooleanLiteralExpr(BooleanLiteralExpr* node) override { 
        return node->value ? "true" : "false"; 
    }
    
    std::string visitBinaryOpExpr(BinaryOpExpr* node) override {
        // Special handling for string equality operations
        if ((node->op == TokenType::EQUAL || node->op == TokenType::NOT_EQUAL)) {
            // Try to determine if operands might be strings
            // This is a simple heuristic - in a real compiler you would use type information
            bool mightBeStringComparison = false;
            
            // Check if either operand is a string literal
            if (auto* left = dynamic_cast<StringLiteralExpr*>(node->left.get())) {
                mightBeStringComparison = true;
            }
            if (auto* right = dynamic_cast<StringLiteralExpr*>(node->right.get())) {
                mightBeStringComparison = true;
            }
            
            // For string comparison, use equals() method
            if (mightBeStringComparison) {
                if (node->op == TokenType::EQUAL) {
                    return "(" + dispatchExpr(node->left.get()) + ".equals(" + 
                           dispatchExpr(node->right.get()) + "))";
                } else { // NOT_EQUAL
                    return "(!(" + dispatchExpr(node->left.get()) + ".equals(" + 
                           dispatchExpr(node->right.get()) + ")))";
                }
            }
        }
        
        // Standard handling for non-string comparisons
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
        if (node->increment) {
             // Increment in Java's for loop is often an expression statement
             // Need dispatchExpr to handle it correctly
            code += dispatchExpr(node->increment.get());
        }
        code += ") {\n";
        indentLevel++;
        if (node->body) code += visitBlock(node->body.get());
        indentLevel--;
        code += getIndent() + "}\n";
        return code;
    }
};
