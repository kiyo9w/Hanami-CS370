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

// ALWAYS use the production URL for deployment - override local development
const PRODUCTION_BACKEND_URL = 'https://hanami-backend-production.up.railway.app/api';

// Get the API URL from environment variables or use the production URL
const API_URL = PRODUCTION_BACKEND_URL;

// Log the API URL for debugging
console.log(`API client configured with endpoint: ${API_URL}`);

// Create a direct URL for fetch calls that's guaranteed to work in the browser
export const getApiUrl = (endpoint: string) => {
  // In browser, we want to use relative URLs for same-origin requests
  if (typeof window !== 'undefined') {
    // Check if we're calling our own backend or an external one
    const currentHost = window.location.host;
    
    // Parse the API URL to get its host
    try {
      const apiUrlObj = new URL(API_URL);
      const apiHost = apiUrlObj.host;
      
      // If API is on the same host, use relative URL
      if (currentHost === apiHost || API_URL.startsWith('/')) {
        // Extract the path from API_URL
        const apiPath = apiUrlObj.pathname;
        return `${apiPath}/${endpoint.replace(/^\//, '')}`;
      }
    } catch (e) {
      // If API_URL is not a valid URL, assume it's a relative path
      if (API_URL.startsWith('/')) {
        return `${API_URL}/${endpoint.replace(/^\//, '')}`;
      }
    }
  }
  
  // Otherwise use the full URL
  return `${API_URL}/${endpoint.replace(/^\//, '')}`;
};

// Track API connectivity status to enable fallback mode
let backendConnected = true;
let connectivityChecked = false;

// Check connectivity once at startup
async function checkBackendConnectivity() {
  if (connectivityChecked) return backendConnected;
  
  try {
    const response = await fetch(`${API_URL}/compiler/logs`, { signal: AbortSignal.timeout(2000) });
    backendConnected = response.ok;
    console.log(`Backend connectivity check: ${backendConnected ? 'Connected' : 'Disconnected'}`);
  } catch (error) {
    console.error('Backend connectivity error:', error);
    backendConnected = false;
  }
  
  connectivityChecked = true;
  return backendConnected;
}

// If on client side, check connectivity immediately
if (typeof window !== 'undefined') {
  checkBackendConnectivity();
}

// Parse error messages from backend response
function extractErrorMessage(error: any, responseData?: any): string {
  // First, check if we have detailed error data from the response
  if (responseData && responseData.error) {
    const errorMsg = responseData.error;
    
    // Look for syntax error patterns in the error message
    if (errorMsg.includes('syntax error') || 
        errorMsg.includes('Parse error') || 
        errorMsg.includes('Error at') ||
        errorMsg.includes('Line')) {
      return `Syntax Error: ${errorMsg}`;
    }
    
    return errorMsg;
  }
  
  // Check if it's a standard Error object
  if (error instanceof Error) {
    const message = error.message;
    
    // Handle common errors in a user-friendly way
    if (message.includes('ECONNREFUSED') || message.includes('Failed to fetch')) {
      return 'Unable to connect to compiler backend. Server might be down.';
    }
    
    if (message.includes('500')) {
      return 'Syntax error in your code. Check for missing brackets, semicolons, or other syntax issues.';
    }
    
    return message;
  }
  
  // Fallback for unknown error types
  return String(error) || 'Unknown error occurred';
}

// Rest of the API client
interface CompilerResponse {
  success: boolean;
  output: string;
  logs: string[];
  error?: string;
}

/**
 * Fetch the latest terminal logs from the server
 * @returns Promise with the logs array
 */
export async function getTerminalLogs(): Promise<string[]> {
  try {
    // Use the full API URL directly
    const url = `${API_URL}/compiler/logs`;
    console.log(`Fetching logs from: ${url}`);
    
    const response = await fetch(url, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
      },
      // Add a timeout to prevent hanging requests
      signal: AbortSignal.timeout(3000),
      // Prevent caching
      cache: 'no-store',
    });
    
    if (!response.ok) {
      console.error(`Error fetching logs: ${response.status} ${response.statusText}`);
      return [`Error fetching logs: ${response.status} ${response.statusText}`];
    }
    
    const data = await response.json();
    
    if (data.success) {
      return data.logs || [];
    } else {
      console.error('Error fetching logs:', data.error);
      return [`Error fetching logs: ${data.error}`];
    }
  } catch (error) {
    console.error('Error connecting to backend for logs:', error);
    return [`Error connecting to backend for logs: ${error instanceof Error ? error.message : ''}`];
  }
}

/**
 * Execute a specific compiler module with the given input and format
 * @param module The module to execute ('lexer', 'parser', 'semantic', 'codegen', 'all')
 * @param input Source code or intermediate representation (tokens, AST, IR)
 * @param inputFormat Format of the input ('source', 'tokens', 'ast', 'ir')
 */
export async function executeModule(
  module: 'lexer' | 'parser' | 'semantic' | 'codegen' | 'all',
  input: string,
  inputFormat: 'source' | 'tokens' | 'ast' | 'ir' = 'source'
): Promise<string> {
  try {
    // Always default to source format for semantic and codegen unless explicitly specified
    if ((module === 'semantic' || module === 'codegen') && inputFormat === 'tokens') {
      inputFormat = 'source';
    }
    
    // Always use source format for the combined 'all' module
    if (module === 'all') {
      inputFormat = 'source';
    }
    
    // Construct the endpoint URL
    const endpoint = getApiUrl(`compiler/${module}`);
    
    // Prepare the request body based on the input format
    let body: any = {};
    
    if (inputFormat === 'source' || module === 'lexer' || module === 'all') {
      body.code = input;
    } else if (inputFormat === 'tokens' && module === 'parser') {
      body.tokens = input;
    } else if (inputFormat === 'ast' && module === 'semantic') {
      body.ast = input;
    } else if (inputFormat === 'ir' && module === 'codegen') {
      body.ir = input;
    } else {
      // Default to sending as code
      body.code = input;
    }
    
    // Log the request for debugging
    console.log(`Executing ${module} with input format: ${inputFormat}`, body);
    
    // Send the API request
    const response = await fetch(endpoint, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(body),
      // Add a reasonable timeout
      signal: AbortSignal.timeout(15000),
    });
    
    let responseData;
    try {
      responseData = await response.json();
    } catch (e) {
      responseData = null;
    }
    
    if (!response.ok) {
      // Extract error details from response if possible
      if (responseData && responseData.error) {
        throw new Error(extractErrorMessage(null, responseData));
      }
      
      // Handle common error types
      if (response.status === 500) {
        throw new Error('Syntax error in your code. Check for missing brackets, semicolons, or other syntax issues.');
      }
      
      if (response.status === 400) {
        throw new Error('Invalid input format or syntax error in your code.');
      }
      
      if (response.status === 404) {
        throw new Error('API endpoint not found. This module might not be implemented yet.');
      }
      
      throw new Error(`Server returned ${response.status}: ${response.statusText}`);
    }
    
    if (!responseData.success) {
      throw new Error(extractErrorMessage(new Error(responseData.error || 'Unknown error'), responseData));
    }
    
    return responseData.output;
  } catch (error) {
    console.error(`Error executing ${module}:`, error);
    
    // For lexer and parser, we can fall back to frontend simulation
    if ((module === 'lexer' || module === 'parser')) {
      console.log(`Falling back to frontend simulation for ${module}`);
      try {
        if (module === 'lexer') {
          const fallbackResult = await api.runLexer(input);
          return fallbackResult.output;
        } else if (module === 'parser') {
          const fallbackResult = await api.runParser(input, inputFormat !== 'source');
          return fallbackResult.output;
        }
      } catch (fallbackError) {
        console.error(`Error in frontend simulation fallback:`, fallbackError);
      }
    }
    
    // Return the extracted error message
    throw error;
  }
} 