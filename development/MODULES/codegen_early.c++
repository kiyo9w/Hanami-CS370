#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum class IRType {
    VAR_DECLARATION,
    ASSIGNMENT,
    BINARY_OP,
    UNARY_OP,
    IF,
    WHILE,
    FUNC_CALL,
    FOR_LOOP
};

struct IRNode {
    IRType type;
    string op;
    string lhs;
    string rhs;
    string condition;
    string func_name;
    vector<string> args;

    string init;
    string loop_cond;
    string iteration;
    vector<IRNode> body;
};

class CodeGenerator {
public:
    void generateCode(const vector<IRNode>& irNodes) {
        for (const auto& node : irNodes) {
            switch (node.type) {
                case IRType::FOR_LOOP:
                    generateForLoop(node);
                    break;
                case IRType::VAR_DECLARATION:
                    cout << node.lhs << " " << node.op << ";" << endl;
                    break;
                case IRType::ASSIGNMENT:
                    cout << node.lhs << " = " << node.rhs << ";" << endl;
                    break;
                case IRType::BINARY_OP:
                    cout << node.lhs << " " << node.op << " " << node.rhs << ";" << endl;
                    break;
                case IRType::UNARY_OP:
                    cout << node.op << node.lhs << ";" << endl;
                    break;
                case IRType::IF:
                    cout << "if (" << node.condition << ") {" << endl
                         << "    // if block code" << endl
                         << "} else {" << endl
                         << "    // else block code" << endl
                         << "}" << endl;
                    break;
                case IRType::WHILE:
                    cout << "while (" << node.condition << ") {" << endl
                         << "    // while loop body" << endl
                         << "}" << endl;
                    break;
                case IRType::FUNC_CALL:
                    cout << node.func_name << "(";
                    printArgs(node.args);
                    cout << ");" << endl;
                    break;
                default:
                    cerr << "Unsupported IR Type!" << endl;
            }
        }
    }

private:
    void printArgs(const vector<string>& args) {
        for (size_t i = 0; i < args.size(); ++i) {
            cout << args[i];
            if (i < args.size() - 1) {
                cout << ", ";
            }
        }
    }

    void generateForLoop(const IRNode& node) {
        cout << "for (" << node.init << "; " << node.loop_cond << "; " << node.iteration << ") {" << endl;

        for (const auto& bodyNode : node.body) {
            generateCode({bodyNode});
        }

        cout << "}" << endl;
    }
};

int main() {
    vector<IRNode> irNodes;

    // Declaration: x
    irNodes.push_back(IRNode{IRType::VAR_DECLARATION, "int", "x", "", "", "", {}});

    // Assignment: x = 5;
    irNodes.push_back(IRNode{IRType::ASSIGNMENT, "", "x", "5", "", "", {}});

    // Binary operation: x + y;
    irNodes.push_back(IRNode{IRType::BINARY_OP, "+", "x", "y", "", "", {}});

    // Unary operation: -x;
    irNodes.push_back(IRNode{IRType::UNARY_OP, "-", "x", "", "", "", {}});

    // If statement: if (x > y) { ... }
    irNodes.push_back(IRNode{IRType::IF, "", "", "", "x > y", "", {}});

    // While loop: while (x < 10) { ... }
    irNodes.push_back(IRNode{IRType::WHILE, "", "", "", "x < 10", "", {}});

    // Function call: foo(a, b);
    irNodes.push_back(IRNode{IRType::FUNC_CALL, "", "", "", "", "foo", {"a", "b"}});

    // For loop structure with initialization, condition, and iteration
    IRNode forLoopNode{IRType::FOR_LOOP, "", "", "", "", "", {}, "a = 9", "a > 0", "a--"};

    // Body of the for loop: assign value 10 to variable 'b'
    IRNode assignNode{IRType::ASSIGNMENT, "", "b", "10", "", "", {}};

    forLoopNode.body.push_back(assignNode);
    irNodes.push_back(forLoopNode);

    CodeGenerator codeGen;
    codeGen.generateCode(irNodes);
    return 0;
}