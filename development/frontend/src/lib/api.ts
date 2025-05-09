/**
 * In a production environment, this would make HTTP requests to a backend API.
 * For this demo, we're simulating the module processing in the frontend.
 */

// Common sample code
const SAMPLE_CODE = `// Simple math and variable test
garden MathTest

grow calculate() -> int {
    int a = 10;
    int b = 5 * (a - 3); // b should be 35
    int c = (a + b) % 8; // c should be 5

    bloom << "Result: " << c << "\\n"; 

    blossom c;
}`

interface ModuleResult {
  output: string
  logs: string[]
}

// Simulate frontend-only processing for now
// These would be replaced with actual API calls in a production environment
export const api = {
  async runLexer(input: string): Promise<ModuleResult> {
    // Simulate processing time and return tokens
    await new Promise(resolve => setTimeout(resolve, 1000))
    
    const logs = [
      'INFO: Starting lexical analysis...',
      'INFO: Processing input code...',
      'INFO: Tokenization complete.',
      'SUCCESS: Lexical analysis completed successfully!'
    ]
    
    const output = `GARDEN 1 1
IDENTIFIER MathTest 1 8
GROW 3 1
IDENTIFIER calculate 3 6
LEFT_PAREN 3 15
RIGHT_PAREN 3 16
ARROW 3 18
IDENTIFIER int 3 21
LEFT_BRACE 3 25
IDENTIFIER int 4 5
IDENTIFIER a 4 9
ASSIGN 4 11
NUMBER 10 4 13
SEMICOLON 4 15
IDENTIFIER int 5 5
IDENTIFIER b 5 9
ASSIGN 5 11
NUMBER 5 5 13
MULTIPLY 5 15
LEFT_PAREN 5 17
IDENTIFIER a 5 18
MINUS 5 20
NUMBER 3 5 22
RIGHT_PAREN 5 23
SEMICOLON 5 24
IDENTIFIER int 6 5
IDENTIFIER c 6 9
ASSIGN 6 11
LEFT_PAREN 6 13
IDENTIFIER a 6 14
PLUS 6 16
IDENTIFIER b 6 18
RIGHT_PAREN 6 19
MODULO 6 21
NUMBER 8 6 23
SEMICOLON 6 24
BLOOM 8 5
STREAM_OUT 8 11
STRING Result:  8 14
STREAM_OUT 8 25
IDENTIFIER c 8 28
STREAM_OUT 8 30
STRING \\n 8 33
SEMICOLON 8 37
BLOSSOM 10 5
IDENTIFIER c 10 13
SEMICOLON 10 14
RIGHT_BRACE 11 1
EOF_TOKEN 11 2`
    
    return { output, logs }
  },
  
  async runParser(input: string, isTokens: boolean = false): Promise<ModuleResult> {
    // Simulate processing time and return AST
    await new Promise(resolve => setTimeout(resolve, 1500))
    
    const logs = [
      'INFO: Starting parsing...',
      'INFO: Tokenizing input...',
      'INFO: Building abstract syntax tree...',
      'SUCCESS: Parsing completed successfully!'
    ]
    
    const output = `{
  "type": "Program",
  "garden": {
    "name": "MathTest",
    "functions": [
      {
        "type": "Function",
        "name": "calculate",
        "returnType": "int",
        "parameters": [],
        "body": {
          "type": "Block",
          "statements": [
            {
              "type": "VariableDeclaration",
              "dataType": "int",
              "name": "a",
              "initializer": {
                "type": "Literal",
                "value": 10
              }
            },
            {
              "type": "VariableDeclaration",
              "dataType": "int",
              "name": "b",
              "initializer": {
                "type": "BinaryExpression",
                "operator": "*",
                "left": {
                  "type": "Literal",
                  "value": 5
                },
                "right": {
                  "type": "BinaryExpression",
                  "operator": "-",
                  "left": {
                    "type": "Identifier",
                    "name": "a"
                  },
                  "right": {
                    "type": "Literal",
                    "value": 3
                  }
                }
              }
            },
            {
              "type": "VariableDeclaration",
              "dataType": "int",
              "name": "c",
              "initializer": {
                "type": "BinaryExpression",
                "operator": "%",
                "left": {
                  "type": "BinaryExpression",
                  "operator": "+",
                  "left": {
                    "type": "Identifier",
                    "name": "a"
                  },
                  "right": {
                    "type": "Identifier",
                    "name": "b"
                  }
                },
                "right": {
                  "type": "Literal",
                  "value": 8
                }
              }
            },
            {
              "type": "OutputStatement",
              "expressions": [
                {
                  "type": "Literal",
                  "value": "Result: "
                },
                {
                  "type": "Identifier",
                  "name": "c"
                },
                {
                  "type": "Literal",
                  "value": "\\n"
                }
              ]
            },
            {
              "type": "ReturnStatement",
              "expression": {
                "type": "Identifier",
                "name": "c"
              }
            }
          ]
        }
      }
    ]
  }
}`
    
    return { output, logs }
  },
  
  async runSemanticAnalyzer(input: string, isAst: boolean = false): Promise<ModuleResult> {
    // Simulate processing time and return semantic analysis
    await new Promise(resolve => setTimeout(resolve, 2000))
    
    const logs = [
      'INFO: Starting semantic analysis...',
      'INFO: Parsing input...',
      'INFO: Building symbol table...',
      'INFO: Checking types...',
      'INFO: Verifying variable usage...',
      'SUCCESS: Semantic analysis completed successfully!'
    ]
    
    const output = `{
  "symbolTable": {
    "global": {
      "MathTest": {
        "type": "garden",
        "scope": "global"
      }
    },
    "MathTest": {
      "calculate": {
        "type": "function",
        "returnType": "int",
        "parameters": [],
        "scope": "MathTest"
      }
    },
    "calculate": {
      "a": {
        "type": "variable",
        "dataType": "int",
        "scope": "calculate"
      },
      "b": {
        "type": "variable",
        "dataType": "int",
        "scope": "calculate"
      },
      "c": {
        "type": "variable",
        "dataType": "int",
        "scope": "calculate"
      }
    }
  },
  "typeChecking": [
    {
      "expression": "a = 10",
      "expectedType": "int",
      "actualType": "int",
      "valid": true
    },
    {
      "expression": "a - 3",
      "expectedType": "int",
      "actualType": "int",
      "valid": true
    },
    {
      "expression": "5 * (a - 3)",
      "expectedType": "int",
      "actualType": "int",
      "valid": true
    },
    {
      "expression": "b = 5 * (a - 3)",
      "expectedType": "int",
      "actualType": "int",
      "valid": true
    },
    {
      "expression": "a + b",
      "expectedType": "int",
      "actualType": "int",
      "valid": true
    },
    {
      "expression": "(a + b) % 8",
      "expectedType": "int",
      "actualType": "int",
      "valid": true
    },
    {
      "expression": "c = (a + b) % 8",
      "expectedType": "int",
      "actualType": "int",
      "valid": true
    },
    {
      "expression": "return c",
      "expectedType": "int",
      "actualType": "int",
      "valid": true
    }
  ],
  "errors": []
}`
    
    return { output, logs }
  },
  
  async runCodeGenerator(input: string, isIr: boolean = false): Promise<ModuleResult> {
    // Simulate processing time and return generated code
    await new Promise(resolve => setTimeout(resolve, 2500))
    
    const logs = [
      'INFO: Starting code generation...',
      'INFO: Parsing input...',
      'INFO: Performing semantic analysis...',
      'INFO: Generating target code...',
      'INFO: Optimizing output...',
      'SUCCESS: Code generation completed successfully!'
    ]
    
    const output = `#include <iostream>
#include <string>

namespace MathTest {
    int calculate() {
        int a = 10;
        int b = 5 * (a - 3); // b should be 35
        int c = (a + b) % 8; // c should be 5

        std::cout << "Result: " << c << "\\n";

        return c;
    }
}

int main() {
    return MathTest::calculate();
}
`
    
    return { output, logs }
  },
  
  async runAll(input: string): Promise<ModuleResult> {
    // Run all modules in sequence
    await new Promise(resolve => setTimeout(resolve, 3000))
    
    const logs = [
      'INFO: Starting complete compilation pipeline...',
      'INFO: Running lexical analysis...',
      'INFO: Lexical analysis completed.',
      'INFO: Running parser...',
      'INFO: Parsing completed.',
      'INFO: Running semantic analyzer...',
      'INFO: Semantic analysis completed.',
      'INFO: Running code generator...',
      'INFO: Code generation completed.',
      'SUCCESS: Compilation completed successfully!',
      'INFO: Executable output would be: Result: 5'
    ]
    
    const output = `# Compilation Pipeline Results

## Lexical Analysis
GARDEN 1 1
IDENTIFIER MathTest 1 8
GROW 3 1
... (additional tokens)

## Abstract Syntax Tree (Simplified)
Program(Garden("MathTest"), [Function("calculate", "int", [], Block([...])])

## Semantic Analysis
No errors found.
All variables are properly declared and used.
All expressions have compatible types.

## Generated Code
\`\`\`cpp
#include <iostream>
#include <string>

namespace MathTest {
    int calculate() {
        int a = 10;
        int b = 5 * (a - 3); // b should be 35
        int c = (a + b) % 8; // c should be 5

        std::cout << "Result: " << c << "\\n";

        return c;
    }
}

int main() {
    return MathTest::calculate();
}
\`\`\`

## Execution Result
Result: 5
`
    
    return { output, logs }
  },
  
  getSampleCode(): string {
    return SAMPLE_CODE
  }
}

/**
 * API client for communicating with the Hanami Compiler backend
 */

const API_URL = process.env.NEXT_PUBLIC_API_URL || 'https://hanami-backend-production.up.railway.app/api';

// Add a console log to show the API URL being used
console.log(`API client configured with endpoint: ${API_URL}`);

interface CompilerResponse {
  success: boolean;
  output: string;
  logs: string[];
  error?: string;
}

/**
 * Run the lexer module
 * @param code Source code to analyze
 */
export async function runLexer(code: string): Promise<CompilerResponse> {
  try {
    const response = await fetch(`${API_URL}/compiler/lexer`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ code }),
    });

    return await response.json();
  } catch (error) {
    console.error('Error calling lexer API:', error);
    return {
      success: false,
      output: '',
      logs: [`Error: Unable to connect to backend server. ${error instanceof Error ? error.message : ''}`],
      error: 'Failed to connect to server',
    };
  }
}

/**
 * Run the parser module
 * @param input Source code or tokens, depending on format
 * @param isTokens Whether the input is already tokens
 */
export async function runParser(input: string, isTokens: boolean = false): Promise<CompilerResponse> {
  try {
    const body = isTokens 
      ? { tokens: input } // Send as tokens if the toggle is on
      : { code: input };  // Otherwise send as code for lexing first
    
    const response = await fetch(`${API_URL}/compiler/parser`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(body),
    });

    return await response.json();
  } catch (error) {
    console.error('Error calling parser API:', error);
    return {
      success: false,
      output: '',
      logs: [`Error: Unable to connect to backend server. ${error instanceof Error ? error.message : ''}`],
      error: 'Failed to connect to server',
    };
  }
}

/**
 * Run the semantic analyzer module
 * @param input Source code or AST, depending on format
 * @param isAst Whether the input is already an AST
 */
export async function runSemanticAnalyzer(input: string, isAst: boolean = false): Promise<CompilerResponse> {
  try {
    const body = isAst 
      ? { ast: input } // Send as AST if the toggle is on
      : { code: input }; // Otherwise send as code for lexing/parsing first
    
    const response = await fetch(`${API_URL}/compiler/semantic`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(body),
    });

    return await response.json();
  } catch (error) {
    console.error('Error calling semantic analyzer API:', error);
    return {
      success: false,
      output: '',
      logs: [`Error: Unable to connect to backend server. ${error instanceof Error ? error.message : ''}`],
      error: 'Failed to connect to server',
    };
  }
}

/**
 * Run the code generator module
 * @param input Source code or IR, depending on format
 * @param isIr Whether the input is already an IR
 */
export async function runCodeGenerator(input: string, isIr: boolean = false): Promise<CompilerResponse> {
  try {
    const body = isIr 
      ? { ir: input } // Send as IR if the toggle is on
      : { code: input }; // Otherwise send as code for full pipeline first
    
    const response = await fetch(`${API_URL}/compiler/codegen`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(body),
    });

    return await response.json();
  } catch (error) {
    console.error('Error calling code generator API:', error);
    return {
      success: false,
      output: '',
      logs: [`Error: Unable to connect to backend server. ${error instanceof Error ? error.message : ''}`],
      error: 'Failed to connect to server',
    };
  }
}

/**
 * Run the full compilation pipeline
 * @param code Source code to compile
 */
export async function runFullCompiler(code: string): Promise<CompilerResponse> {
  try {
    const response = await fetch(`${API_URL}/compiler/run`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ code }),
    });

    return await response.json();
  } catch (error) {
    console.error('Error calling full compiler API:', error);
    return {
      success: false,
      output: '',
      logs: [`Error: Unable to connect to backend server. ${error instanceof Error ? error.message : ''}`],
      error: 'Failed to connect to server',
    };
  }
}

/**
 * Fetch the latest terminal logs from the server
 * @returns Promise with the logs array
 */
export async function getTerminalLogs(): Promise<string[]> {
  try {
    const response = await fetch(`${API_URL}/compiler/logs`);
    const data = await response.json();
    
    if (data.success) {
      return data.logs;
    } else {
      console.error('Error fetching logs:', data.error);
      return [`Error fetching logs: ${data.error}`];
    }
  } catch (error) {
    console.error('Error connecting to backend for logs:', error);
    return [`Error connecting to backend for logs: ${error instanceof Error ? error.message : ''}`];
  }
} 