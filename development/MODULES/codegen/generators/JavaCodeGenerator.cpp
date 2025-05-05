// Included by codegen/main.cpp

#include <string>
#include <sstream>
#include <stdexcept>
#include <map>

class JavaCodeGenerator : public CodeGeneratorVisitor {
public:
    std::string generate(ASTNode* node) override {
        // Reset state for new generation if needed
        generatedCode_.str(""); 
        generatedCode_.clear();
        std::string gardenName = ""; // Find garden name
        hasMain_ = false;
        indentLevel = 0;
        currentSpeciesName_ = ""; // Reset context
        
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
        
        // Add the final closing brace for the main class
        indentLevel--; 
        generatedCode_ << getIndent() << "}\n";
        
        return generatedCode_.str();
    }

    // Getter for the determined class name
    std::string getClassName() const {
        return className_; // Return the name calculated in generate()
    }

private:
    std::stringstream generatedCode_;
    std::string className_ = "GeneratedHanamiClass";
    std::string currentSpeciesName_ = ""; // Track context
    bool hasMain_ = false;
    int indentLevel = 0;
    // Add a map to store variable types (simple simulation of symbol table info)
    std::map<std::string, std::string> variableTypes_; 
    // Map to store member types for each species
    std::map<std::string, std::map<std::string, std::string>> speciesMemberTypes_;
    
    // Helper method to get proper indentation
    std::string getIndent() {
        return std::string(indentLevel * 4, ' ');
    }
    
    // --- Type Mapping --- 
    std::string mapType(const std::string& hanamiType) {
        if (hanamiType == "int") return "int";
        if (hanamiType == "bool") return "boolean";  // Java uses boolean
        if (hanamiType == "string") return "String";  // Java uses String with capital S
        if (hanamiType == "void") return "void";
        if (hanamiType == "float") return "float";
        if (hanamiType == "double") return "double";
        
        // Capitalize species name for Java class name conventions
        if (!hanamiType.empty()) {
            std::string capitalized = hanamiType;
            capitalized[0] = std::toupper(capitalized[0]);
            return capitalized;  // Assume species name should be a Java class
        }
        
        return hanamiType;  // Fallback
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
        
        std::string previousSpeciesName = currentSpeciesName_;
        currentSpeciesName_ = node->name; // Set context
        
        indentLevel++;
        
        // First pass: Collect member variable types for this species
        if (speciesMemberTypes_.find(node->name) == speciesMemberTypes_.end()) {
            speciesMemberTypes_[node->name] = {}; // Ensure map entry exists
        }
        for (const auto& section : node->sections) {
            if (auto* visBlock = dynamic_cast<VisibilityBlockStmt*>(section.get())) {
                if(visBlock->block){
                    for (const auto& stmt : visBlock->block->statements) {
                        if (auto* varDecl = dynamic_cast<VariableDeclStmt*>(stmt.get())) {
                            speciesMemberTypes_[node->name][varDecl->varName] = mapType(varDecl->typeName);
                        }
                    }
                }
            }
        }
        
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
                           // Store member types before visiting body
                           if (!node->name.empty() && speciesMemberTypes_.find(node->name) == speciesMemberTypes_.end()) {
                              speciesMemberTypes_[node->name] = {}; // Ensure map exists
                           }
                           for(const auto& bodyStmt : funcDef->body->statements){
                               if (auto* memberVarDecl = dynamic_cast<VariableDeclStmt*>(bodyStmt.get())){
                                   speciesMemberTypes_[node->name][memberVarDecl->varName] = mapType(memberVarDecl->typeName);
                               }
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
        
        currentSpeciesName_ = previousSpeciesName; // Restore context
        
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
        // Store type for later lookup (e.g., for input parsing)
        variableTypes_[node->varName] = mapType(node->typeName);

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
        std::string code = ""; // Start empty, add indent per line
        variableTypes_.clear(); // Clear types for new function scope

        // Check if this function should be the Java main method
        bool isJavaMain = (currentSpeciesName_.empty() && node->name == "mainGarden");

        if (isJavaMain) {
            hasMain_ = true;
            // Ensure it's public static void main(String[] args)
            code += getIndent() + "public static void main(String[] args) {\n";
            indentLevel++;

            // Define parameters in variableTypes_ map for main (though usually none)
            for (const auto& param : node->parameters) {
                 variableTypes_[param.paramName] = mapType(param.typeName);
            }

            // Add function body
            if (node->body) {
                for (const auto& stmt_ptr : node->body->statements) {
                    // Special handling for return in main -> System.exit()
                    if (auto* returnStmt = dynamic_cast<ReturnStmt*>(stmt_ptr.get())) {
                        if (returnStmt->returnValue) {
                            code += getIndent() + "System.exit(" + dispatchExpr(returnStmt->returnValue.get()) + ");\n";
                        } else {
                            code += getIndent() + "System.exit(0);\n";
                        }
                    } else {
                        // Need to visit the statement to populate variableTypes_ before potential use in IO
                        // Store the generated code from dispatch temporarily
                        std::string stmtCode = dispatch(stmt_ptr.get());
                        code += stmtCode; // Append generated code for the statement
                    }
                }
            }

            indentLevel--;
            code += getIndent() + "}\n\n";
            return code;
        } else { // Handle normal functions/methods
             bool isStatic = currentSpeciesName_.empty(); // Standalone functions are static
            code += getIndent();
            // Determine visibility (simplified: public for now)
            code += "public ";
            if (isStatic) code += "static ";
            code += mapType(node->returnType) + " " + node->name + "(";
            // Add parameters and store their types
            for (size_t i = 0; i < node->parameters.size(); ++i) {
                variableTypes_[node->parameters[i].paramName] = mapType(node->parameters[i].typeName);
                code += mapType(node->parameters[i].typeName) + " " + node->parameters[i].paramName;
                if (i < node->parameters.size() - 1) code += ", ";
            }
            code += ") {\n";

            indentLevel++;

            // Add function body
            if (node->body) {
                // Use visitBlock which will dispatch individual statements
                code += visitBlock(node->body.get());
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
        std::string code = ""; // Start empty, add indent per line

        if (node->ioType == TokenType::BLOOM) { // Output
            code += getIndent() + "System.out.print("; // Add indent here
            for (size_t i = 0; i < node->expressions.size(); ++i) {
                std::string exprCode = dispatchExpr(node->expressions[i].get());
                // Use the result from dispatchExpr directly (escaping handled by visitStringLiteralExpr)
                code += exprCode; 
                if (i < node->expressions.size() - 1) code += " + "; // Concatenate for print
            }
            code += ");\n"; // Use print, handle println via \n in string literal
        } else if (node->ioType == TokenType::WATER) { // Input
            for (const auto& expr : node->expressions) {
                 std::string lineCode = getIndent(); // Indent each assignment line
                if (IdentifierExpr* ident = dynamic_cast<IdentifierExpr*>(expr.get())) {
                    std::string varName = ident->name;
                    std::string targetType = "";

                    // Check if it's a known member of the current species
                     std::string mappedSpeciesName = mapType(currentSpeciesName_);
                    if (!currentSpeciesName_.empty() && speciesMemberTypes_.count(mappedSpeciesName) && speciesMemberTypes_[mappedSpeciesName].count(varName)) {
                        targetType = speciesMemberTypes_[mappedSpeciesName][varName];
                        lineCode += "this."; // Assuming member access requires 'this.' implicitly
                    }
                    // Otherwise, check if it's a local variable or parameter
                    else if (variableTypes_.count(varName)) {
                        targetType = variableTypes_[varName];
                    }
                     // If still unknown, targetType remains ""

                    lineCode += varName + " = ";
                    if (targetType == "int") {
                        lineCode += "Integer.parseInt(inputScanner.nextLine());\n";
                    } else if (targetType == "float") {
                        lineCode += "Float.parseFloat(inputScanner.nextLine());\n";
                    } else if (targetType == "double") {
                        lineCode += "Double.parseDouble(inputScanner.nextLine());\n";
                    } else if (targetType == "boolean") {
                         lineCode += "Boolean.parseBoolean(inputScanner.nextLine());\n";
                    } else { // Default to String or completely unknown type
                         lineCode += "inputScanner.nextLine(); // Assumed String or unknown type ('"+targetType+"')\n";
                    }
                } else if (MemberAccessExpr* member = dynamic_cast<MemberAccessExpr*>(expr.get())) {
                    // Determine object type (using heuristic)
                    std::string objectTypeName = "";
                     // First check if the object is an identifier we know the type of
                    if(IdentifierExpr* objIdent = dynamic_cast<IdentifierExpr*>(member->object.get())){
                        if(variableTypes_.count(objIdent->name)){
                            objectTypeName = variableTypes_[objIdent->name]; // Get mapped Java type
                        }
                    }
                     // If we couldn't determine type, objectTypeName remains ""

                    std::string objectName = dispatchExpr(member->object.get());
                    std::string memberName = member->member->name;
                    std::string memberType = ""; // Default to unknown

                    // Lookup member type in the species map using the determined object type name
                    if (!objectTypeName.empty() && speciesMemberTypes_.count(objectTypeName)) {
                         if(speciesMemberTypes_[objectTypeName].count(memberName)){
                             memberType = speciesMemberTypes_[objectTypeName][memberName];
                         }
                    }

                    // Generate assignment with appropriate parser
                    lineCode += objectName + "." + memberName + " = ";
                    if (memberType == "int") {
                        lineCode += "Integer.parseInt(inputScanner.nextLine());\n";
                    } else if (memberType == "float") {
                        lineCode += "Float.parseFloat(inputScanner.nextLine());\n";
                    } else if (memberType == "double") {
                        lineCode += "Double.parseDouble(inputScanner.nextLine());\n";
                    } else if (memberType == "boolean") {
                         lineCode += "Boolean.parseBoolean(inputScanner.nextLine());\n";
                    } else { // Default to String or unknown type
                         lineCode += "inputScanner.nextLine(); // Reads as String, member type ('" + memberType + "') unknown or String\n";
                    }
                } else {
                    lineCode += "/* Error: Cannot read input into non-variable */\n";
                }
                code += lineCode; // Append the generated line
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
        else if (auto* flt = dynamic_cast<FloatLiteralExpr*>(expr)) {
            return visitFloatLiteralExpr(flt);
        }
        else if (auto* dbl = dynamic_cast<DoubleLiteralExpr*>(expr)) {
            return visitDoubleLiteralExpr(dbl);
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
        std::string escapedValue = "";
        for (char c : node->value) {
            switch (c) {
                case '\\': escapedValue += "\\\\"; break; // Backslash escape
                case '"': escapedValue += "\\\""; break; // Double quote escape
                case '\n': escapedValue += "\\n"; break; // Newline escape
                // Removed other cases
                default:   escapedValue += c; break; // Corrected variable name
            }
        }
        return "\"" + escapedValue + "\""; 
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
