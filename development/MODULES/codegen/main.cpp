#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <map>
#include <iomanip> // For file output formatting if needed

#include "../common/token.h" // May need TokenType if IR uses it
#include "../common/ast.h"   // Needs AST node definitions to parse IR
#include "../common/json.hpp" 
#include "../common/json_deserializer.h" // Use the shared deserializer

// --- JSON Deserialization --- 
// Remove placeholder deserialization functions
// All deserialization is now handled by the functions declared in json_deserializer.h
// and implemented in json_deserializer.cpp

// --- Code Generation Logic (Placeholders) --- 

// TODO: Implement code generation visitors/functions for each language
// Forward declare visitors
class JavaCodeGenerator;
class PythonCodeGenerator;
class CppCodeGenerator;
class JavaScriptCodeGenerator;

// --- Base Code Generator Visitor (using accept pattern) ---
// Add accept methods to AST nodes if not already present
// Modify ast.h if needed, e.g.:
/*
struct Expression {
    virtual ~Expression() = default;
    virtual std::string accept(CodeGeneratorVisitor& visitor) = 0; 
    ...
};
*/
// For now, we'll use dynamic_cast in the visitor.

class CodeGeneratorVisitor {
public:
    virtual ~CodeGeneratorVisitor() = default;
    virtual std::string generate(ASTNode* node) = 0;
    
protected:
    // Common utilities or state if needed
    int indentLevel = 0;
    std::string getIndent() {
        return std::string(indentLevel * 4, ' ');
    }
    
    // Visitor methods to be implemented by subclasses
    virtual std::string visitProgram(ProgramNode* node) = 0;
    virtual std::string visitStyleInclude(StyleIncludeStmt* node) = 0;
    virtual std::string visitGardenDecl(GardenDeclStmt* node) = 0;
    virtual std::string visitSpeciesDecl(SpeciesDeclStmt* node) = 0;
    virtual std::string visitVisibilityBlock(VisibilityBlockStmt* node) = 0;
    virtual std::string visitBlock(BlockStmt* node) = 0;
    virtual std::string visitVariableDecl(VariableDeclStmt* node) = 0;
    virtual std::string visitFunctionDef(FunctionDefStmt* node) = 0;
    virtual std::string visitReturn(ReturnStmt* node) = 0;
    virtual std::string visitExpressionStmt(ExpressionStmt* node) = 0;
    virtual std::string visitBranch(BranchStmt* node) = 0;
    virtual std::string visitIO(IOStmt* node) = 0;
    
    virtual std::string visitWhileStmt(WhileStmt* node) = 0;
    virtual std::string visitForStmt(ForStmt* node) = 0;
    
    virtual std::string visitIdentifierExpr(IdentifierExpr* node) = 0;
    virtual std::string visitNumberLiteralExpr(NumberLiteralExpr* node) = 0;
    virtual std::string visitStringLiteralExpr(StringLiteralExpr* node) = 0;
    virtual std::string visitBooleanLiteralExpr(BooleanLiteralExpr* node) = 0;
    virtual std::string visitBinaryOpExpr(BinaryOpExpr* node) = 0;
    virtual std::string visitFunctionCallExpr(FunctionCallExpr* node) = 0;
    virtual std::string visitMemberAccessExpr(MemberAccessExpr* node) = 0;
    virtual std::string visitAssignmentStmt(AssignmentStmt* node) = 0;
    
    // Generic dispatch using dynamic_cast
     std::string dispatch(ASTNode* node) {
         if (!node) return "";
         if (auto* p = dynamic_cast<ProgramNode*>(node)) return visitProgram(p);
         if (auto* p = dynamic_cast<StyleIncludeStmt*>(node)) return visitStyleInclude(p);
         if (auto* p = dynamic_cast<GardenDeclStmt*>(node)) return visitGardenDecl(p);
         if (auto* p = dynamic_cast<SpeciesDeclStmt*>(node)) return visitSpeciesDecl(p);
         if (auto* p = dynamic_cast<VisibilityBlockStmt*>(node)) return visitVisibilityBlock(p);
         if (auto* p = dynamic_cast<BlockStmt*>(node)) return visitBlock(p);
         if (auto* p = dynamic_cast<VariableDeclStmt*>(node)) return visitVariableDecl(p);
         if (auto* p = dynamic_cast<FunctionDefStmt*>(node)) return visitFunctionDef(p);
         if (auto* p = dynamic_cast<ReturnStmt*>(node)) return visitReturn(p);
         if (auto* p = dynamic_cast<ExpressionStmt*>(node)) return visitExpressionStmt(p);
         if (auto* p = dynamic_cast<BranchStmt*>(node)) return visitBranch(p);
         if (auto* p = dynamic_cast<IOStmt*>(node)) return visitIO(p);
         
         if (auto* p = dynamic_cast<WhileStmt*>(node)) return visitWhileStmt(p);
         if (auto* p = dynamic_cast<ForStmt*>(node)) return visitForStmt(p);
         
         // Expressions (need to handle them within statements/other expressions)
         if (auto* p = dynamic_cast<IdentifierExpr*>(node)) return visitIdentifierExpr(p);
         if (auto* p = dynamic_cast<NumberLiteralExpr*>(node)) return visitNumberLiteralExpr(p);
         if (auto* p = dynamic_cast<StringLiteralExpr*>(node)) return visitStringLiteralExpr(p);
         if (auto* p = dynamic_cast<BooleanLiteralExpr*>(node)) return visitBooleanLiteralExpr(p);
         if (auto* p = dynamic_cast<BinaryOpExpr*>(node)) return visitBinaryOpExpr(p);
         if (auto* p = dynamic_cast<FunctionCallExpr*>(node)) return visitFunctionCallExpr(p);
         if (auto* p = dynamic_cast<MemberAccessExpr*>(node)) return visitMemberAccessExpr(p);
         if (auto* p = dynamic_cast<AssignmentStmt*>(node)) return visitAssignmentStmt(p);
         
         std::cerr << "Error: CodeGen dispatch failed for node type." << std::endl;
         return "/* Error: Unsupported Node */";
     }
     
     std::string dispatchExpr(Expression* node) {
          // Specifically for expressions, ensures we don't accidentally dispatch statements
           if (!node) return "";
           if (auto* p = dynamic_cast<IdentifierExpr*>(node)) return visitIdentifierExpr(p);
           if (auto* p = dynamic_cast<NumberLiteralExpr*>(node)) return visitNumberLiteralExpr(p);
           if (auto* p = dynamic_cast<StringLiteralExpr*>(node)) return visitStringLiteralExpr(p);
           if (auto* p = dynamic_cast<BooleanLiteralExpr*>(node)) return visitBooleanLiteralExpr(p);
           if (auto* p = dynamic_cast<BinaryOpExpr*>(node)) return visitBinaryOpExpr(p);
           if (auto* p = dynamic_cast<FunctionCallExpr*>(node)) return visitFunctionCallExpr(p);
           if (auto* p = dynamic_cast<MemberAccessExpr*>(node)) return visitMemberAccessExpr(p);
           if (auto* p = dynamic_cast<AssignmentStmt*>(node)) return visitAssignmentStmt(p);
           
           std::cerr << "Error: CodeGen dispatch failed for expression type." << std::endl;
           return "/* Error: Unsupported Expression */";
     }
};

// --- Language-Specific Generators (Implementations) --- 

// TODO: Implement the visit methods for each language

#include "generators/JavaCodeGenerator.cpp" 
#include "generators/PythonCodeGenerator.cpp"
#include "generators/CppCodeGenerator.cpp"
#include "generators/JavaScriptCodeGenerator.cpp"

// Helper to write string content to a file
bool writeToFile(const std::string& filename, const std::string& content) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error: Could not open output file: " << filename << std::endl;
        return false;
    }
    outFile << content;
    outFile.close();
    std::cout << "Successfully wrote code to " << filename << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    std::string inputFilename = "input/input.ir"; // Default input IR file
    std::string outputDir = "output/";        // Default output directory

    if (argc > 1) {
        inputFilename = std::string(argv[1]);
    }
    // Optional: Specify output directory?
    // if (argc > 2) { outputDir = std::string(argv[2]); }

    std::cout << "Code Generation Module" << std::endl;
    std::cout << "Reading IR from: " << inputFilename << std::endl;

    // Read JSON IR file
    std::ifstream inFile(inputFilename);
    if (!inFile) {
        std::cerr << "Error: Could not open input IR file: " << inputFilename << std::endl;
        return 1;
    }
    nlohmann::json irJson;
    try {
        inFile >> irJson;
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "Error: Failed to parse input IR JSON: " << e.what() << std::endl;
        return 1;
    }
    inFile.close();

    std::cout << "Deserializing IR (AST)..." << std::endl;
    std::unique_ptr<ASTNode> astRoot = nullptr;
    try {
         // Use the shared deserializer
         astRoot = fromJson(irJson); 
    } catch (const std::exception& e) {
         std::cerr << "Error during IR deserialization: " << e.what() << std::endl;
         return 1;
    }
    
    if (!astRoot) {
        std::cerr << "Error: Failed to deserialize IR from JSON." << std::endl;
        return 1;
    }
     // Expect ProgramNode as root
     ProgramNode* programRoot = dynamic_cast<ProgramNode*>(astRoot.get());
     if (!programRoot) {
          std::cerr << "Error: Deserialized IR root is not a ProgramNode." << std::endl;
          return 1;
     }

    std::cout << "Generating code for multiple languages..." << std::endl;
    bool success = true;

    // Create generator instances
    JavaCodeGenerator javaGen;
    PythonCodeGenerator pythonGen;
    CppCodeGenerator cppGen;
    JavaScriptCodeGenerator jsGen;

    // Generate Java
    std::string javaCode = javaGen.generate(programRoot);
    success &= writeToFile(outputDir + "output.java", javaCode);

    // Generate Python
    std::string pythonCode = pythonGen.generate(programRoot);
    success &= writeToFile(outputDir + "output.py", pythonCode);

    // Generate C++
    std::string cppCode = cppGen.generate(programRoot);
    success &= writeToFile(outputDir + "output.cpp", cppCode);

    // Generate JavaScript
    std::string jsCode = jsGen.generate(programRoot);
    success &= writeToFile(outputDir + "output.js", jsCode);

    if (!success) {
        std::cerr << "Code generation failed for one or more languages." << std::endl;
        return 1;
    }

    std::cout << "Code generation completed successfully." << std::endl;

    return 0;
} 