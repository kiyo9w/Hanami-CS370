#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum NodeType { ASSIGNMENT, BINARY_OP, IDENTIFIER, NUMBER, FUNCTION };
enum DataType { INT_TYPE, UNKNOWN_TYPE };

// Base AST node
struct ASTNode {
    NodeType type;
    DataType dataType;
    ASTNode(NodeType t) : type(t), dataType(UNKNOWN_TYPE) {}
    virtual ~ASTNode() {} // Virtual destructor for proper cleanup
};

// Identifier node (e.g., a, b, c, d, e, f)
struct IdentifierNode : public ASTNode {
    string name;
    IdentifierNode(const string& n) : ASTNode(IDENTIFIER), name(n) {}
};

// Number node (supports integer numbers)
struct NumberNode : public ASTNode {
    int value;
    NumberNode(int v) : ASTNode(NUMBER), value(v) {
        dataType = INT_TYPE;
    }
};

// Binary operation node (e.g., b + c, c * d, A - E)
struct BinaryOpNode : public ASTNode {
    char op;
    ASTNode* left;
    ASTNode* right;
    BinaryOpNode(char o, ASTNode* l, ASTNode* r)
        : ASTNode(BINARY_OP), op(o), left(l), right(r) {}
    ~BinaryOpNode() {
        delete left;
        delete right;
    }
};

// Assignment node (e.g., a = b + c * d)
struct AssignmentNode : public ASTNode {
    IdentifierNode* left;
    ASTNode* right;
    AssignmentNode(IdentifierNode* l, ASTNode* r)
        : ASTNode(ASSIGNMENT), left(l), right(r) {}
    ~AssignmentNode() {
        delete left;
        delete right;
    }
};

// Function node (e.g., f: y = 3 * x)
struct FunctionNode : public ASTNode {
    string name;                  // Tên của function (e.g., "f")
    vector<IdentifierNode*> parameters; // Các tham số (e.g., "x")
    AssignmentNode* body;         // Nội dung của function (e.g., "y = 3 * x")

    FunctionNode(const string& n, AssignmentNode* b)
        : ASTNode(FUNCTION), name(n), body(b) {}

    // Thêm tham số vào function
    void addParameter(IdentifierNode* param) {
        parameters.push_back(param);
    }

    ~FunctionNode() {
        delete body;
        for (auto param : parameters) {
            delete param;
        }
    }
};

// Symbol table entry structure
struct SymbolEntry {
    string name;
    DataType type;  // Type information included
    string scope;
    int memAddress;
    bool isFunction;  // Flag to indicate if this is a function
};

// A simple static symbol table using a fixed array
const int MAX_SYMBOLS = 100;
SymbolEntry symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

bool symbolExists(const string& name) {
    for (int i = 0; i < symbolCount; i++) {
        if (symbolTable[i].name == name)
            return true;
    }
    return false;
}

void addSymbol(const string& name, DataType type, const string& scope, int memAddress, bool isFunction = false) {
    if (symbolCount < MAX_SYMBOLS) {
        symbolTable[symbolCount].name = name;
        symbolTable[symbolCount].type = type;
        symbolTable[symbolCount].scope = scope;
        symbolTable[symbolCount].memAddress = memAddress;
        symbolTable[symbolCount].isFunction = isFunction;
        symbolCount++;
    }
}

void analyze(ASTNode* node, const string& scope = "global") {
    if (!node)
        return;
    switch (node->type) {
    case FUNCTION: {
        FunctionNode* fn = (FunctionNode*)node;
        // Add function to symbol table
        if (!symbolExists(fn->name))
            addSymbol(fn->name, INT_TYPE, "global", 0, true);

        // Create a new scope for the function parameters
        string functionScope = fn->name;

        // Add parameters to symbol table with function scope
        for (auto param : fn->parameters) {
            if (!symbolExists(param->name + "@" + functionScope))
                addSymbol(param->name, INT_TYPE, functionScope, 0);
            param->dataType = INT_TYPE;
        }

        // Analyze function body in function scope
        analyze(fn->body, functionScope);
        node->dataType = fn->body->dataType;
        break;
    }
    case ASSIGNMENT: {
        AssignmentNode* an = (AssignmentNode*)node;
        // Left side must be an identifier. Add to symbol table if not present.
        string symbolName = an->left->name;
        if (scope != "global") {
            symbolName += "@" + scope;
        }
        if (!symbolExists(symbolName))
            addSymbol(an->left->name, INT_TYPE, scope, 0);
        analyze(an->right, scope);
        node->dataType = INT_TYPE;
        break;
    }
    case BINARY_OP: {
        BinaryOpNode* bn = (BinaryOpNode*)node;
        analyze(bn->left, scope);
        analyze(bn->right, scope);
        if (bn->left->dataType == INT_TYPE && bn->right->dataType == INT_TYPE)
            bn->dataType = INT_TYPE;
        else {
            cout << "Type error in binary operation." << endl;
            bn->dataType = UNKNOWN_TYPE;
        }
        break;
    }
    case IDENTIFIER: {
        IdentifierNode* id = (IdentifierNode*)node;
        string symbolName = id->name;
        if (scope != "global") {
            // Check if this identifier exists in the current scope
            string scopedName = id->name + "@" + scope;
            if (symbolExists(scopedName)) {
                symbolName = scopedName;
            }
        }
        if (!symbolExists(symbolName))
            addSymbol(id->name, INT_TYPE, scope, 0);
        id->dataType = INT_TYPE;
        break;
    }
    case NUMBER: {
        // Data type is already set in the constructor
        break;
    }
    }
}

// Helper function to print indentation
void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        cout << "  ";
    }
}

// printFull prints the complete AST tree structure with node type and data type details.
void printFull(ASTNode* node, int indent = 0) {
    if (!node)
        return;
    printIndent(indent);
    // Print node type and data type
    switch (node->type) {
    case FUNCTION: {
        FunctionNode* fn = (FunctionNode*)node;
        cout << "FUNCTION '" << fn->name << "' (DataType: " << (node->dataType == INT_TYPE ? "int" : "unknown") << ")\n";
        printIndent(indent + 1);
        cout << "Parameters:\n";
        for (auto param : fn->parameters) {
            printFull(param, indent + 2);
        }
        printIndent(indent + 1);
        cout << "Body:\n";
        printFull(fn->body, indent + 2);
        break;
    }
    case ASSIGNMENT: {
        cout << "ASSIGNMENT (DataType: " << (node->dataType == INT_TYPE ? "int" : "unknown") << ")\n";
        AssignmentNode* an = (AssignmentNode*)node;
        printIndent(indent + 1);
        cout << "Left:\n";
        printFull(an->left, indent + 2);
        printIndent(indent + 1);
        cout << "Right:\n";
        printFull(an->right, indent + 2);
        break;
    }
    case BINARY_OP: {
        BinaryOpNode* bn = (BinaryOpNode*)node;
        cout << "BINARY_OP '" << bn->op << "' (DataType: " << (node->dataType == INT_TYPE ? "int" : "unknown") << ")\n";
        printIndent(indent + 1);
        cout << "Left:\n";
        printFull(bn->left, indent + 2);
        printIndent(indent + 1);
        cout << "Right:\n";
        printFull(bn->right, indent + 2);
        break;
    }
    case IDENTIFIER: {
        IdentifierNode* id = (IdentifierNode*)node;
        cout << "IDENTIFIER '" << id->name << "' (DataType: " << (node->dataType == INT_TYPE ? "int" : "unknown") << ")\n";
        break;
    }
    case NUMBER: {
        NumberNode* num = (NumberNode*)node;
        cout << "NUMBER (" << num->value << ") (DataType: " << (node->dataType == INT_TYPE ? "int" : "unknown") << ")\n";
        break;
    }
    }
}

// Intermediate Representation (IR) generation (three-address code)
int tempCount = 0;
string newTemp() {
    return "t" + to_string(tempCount++);
}

string generateIR(ASTNode* node) {
    if (!node)
        return "";
    switch (node->type) {
    case FUNCTION: {
        FunctionNode* fn = (FunctionNode*)node;
        cout << "function " << fn->name << "(";
        // Output parameters
        for (size_t i = 0; i < fn->parameters.size(); i++) {
            cout << fn->parameters[i]->name;
            if (i < fn->parameters.size() - 1) cout << ", ";
        }
        cout << ") {\n";
        // Generate IR for function body
        generateIR(fn->body);
        cout << "}\n";
        return fn->name;
    }
    case ASSIGNMENT: {
        AssignmentNode* an = (AssignmentNode*)node;
        string rightVal = generateIR(an->right);
        cout << an->left->name << " = " << rightVal << "\n";
        return an->left->name;
    }
    case BINARY_OP: {
        BinaryOpNode* bn = (BinaryOpNode*)node;
        string leftVal = generateIR(bn->left);
        string rightVal = generateIR(bn->right);
        string temp = newTemp();
        cout << temp << " = " << leftVal << " " << bn->op << " " << rightVal << "\n";
        return temp;
    }
    case IDENTIFIER: {
        IdentifierNode* id = (IdentifierNode*)node;
        return id->name;
    }
    case NUMBER: {
        NumberNode* num = (NumberNode*)node;
        return to_string(num->value);
    }
    }
    return "";
}

int main() {
    // Build AST for the expression: a = b + c * d
    IdentifierNode* idA = new IdentifierNode("a");
    IdentifierNode* idB = new IdentifierNode("b");
    IdentifierNode* idC = new IdentifierNode("c");
    IdentifierNode* idD = new IdentifierNode("d");
    IdentifierNode* idE = new IdentifierNode("e");
    IdentifierNode* idF = new IdentifierNode("f");

    // Build AST for the expression c * d
    BinaryOpNode* multNode = new BinaryOpNode('*', idC, idD);
    // Build AST for the expression b + (c * d)
    BinaryOpNode* addNode = new BinaryOpNode('+', idB, multNode);
    // Build AST for the expression a = b + c * d
    AssignmentNode* assignNode = new AssignmentNode(new IdentifierNode("a"), addNode);

    // Build AST for the expression A - E, then f = A - E
    BinaryOpNode* minusNode = new BinaryOpNode('-', new IdentifierNode("a"), idE);
    AssignmentNode* assignNode2 = new AssignmentNode(new IdentifierNode("f"), minusNode);

    // Create a new function f: y = 3 * x
    IdentifierNode* idX = new IdentifierNode("x");
    IdentifierNode* idY = new IdentifierNode("y");
    NumberNode* num3 = new NumberNode(3);
    BinaryOpNode* mult3X = new BinaryOpNode('*', num3, new IdentifierNode("x"));
    AssignmentNode* yEquals3X = new AssignmentNode(new IdentifierNode("y"), mult3X);

    FunctionNode* functionF = new FunctionNode("f", yEquals3X);
    functionF->addParameter(new IdentifierNode("x"));

    // Semantic analysis: type checking and symbol table construction
    analyze(assignNode);
    analyze(assignNode2);
    analyze(functionF);

    // Generate Intermediate Representation (IR)
    cout << "Intermediate Representation (IR):\n";
    generateIR(assignNode);
    generateIR(assignNode2);
    generateIR(functionF);

    // Print symbol table with type information
    cout << "\nSymbol Table:\n";
    for (int i = 0; i < symbolCount; i++) {
        cout << "Identifier: " << symbolTable[i].name
            << ", Type: " << (symbolTable[i].type == INT_TYPE ? "int" : "unknown")
            << ", Scope: " << symbolTable[i].scope
            << ", Address: " << symbolTable[i].memAddress
            << ", IsFunction: " << (symbolTable[i].isFunction ? "true" : "false") << "\n";
    }

    // Print full AST structures for both assignment trees
    cout << "\nFull AST for assignNode:\n";
    printFull(assignNode);
    cout << "\nFull AST for assignNode2:\n";
    printFull(assignNode2);
    cout << "\nFull AST for functionF:\n";
    printFull(functionF);

    // Clean up AST memory
    delete assignNode;
    delete assignNode2;
    delete functionF;

    return 0;
}