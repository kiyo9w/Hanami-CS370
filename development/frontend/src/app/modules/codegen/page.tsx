'use client'

import { useState, useEffect } from 'react'
import ModuleLayout from '@/components/ModuleLayout'
import { runCodeGenerator } from '@/lib/api'
import MultiCodeOutput from '@/components/MultiCodeOutput'
import OutputDisplay from '@/components/OutputDisplay'

// Sample Hanami code for demo with a more complex example
const sampleCode = `
garden Example {
  // Simple calculator function to demonstrate code generation
  grow add(int a, int b) -> int {
    blossom a + b;
  }
  
  grow subtract(int a, int b) -> int {
    blossom a - b;
  }
  
  grow multiply(int a, int b) -> int {
    blossom a * b;
  }
  
  grow main() -> int {
    int x = 10;
    int y = 5;
    
    bloom << "Addition: " << add(x, y) << "\\n";
    bloom << "Subtraction: " << subtract(x, y) << "\\n";
    bloom << "Multiplication: " << multiply(x, y) << "\\n";
    
    blossom 0;
  }
}
`;

// Sample IR for code generator (from semantic analyzer)
const sampleIR = `{
  "symbolTable": {
    "global": {
      "Example": { "type": "garden", "scope": "global" }
    },
    "Example": {
      "add": { "type": "function", "returnType": "int", "parameters": ["int", "int"], "scope": "Example" },
      "subtract": { "type": "function", "returnType": "int", "parameters": ["int", "int"], "scope": "Example" },
      "multiply": { "type": "function", "returnType": "int", "parameters": ["int", "int"], "scope": "Example" },
      "main": { "type": "function", "returnType": "int", "parameters": [], "scope": "Example" }
    }
  },
  "ast": {
    "type": "Program",
    "garden": {
      "name": "Example",
      "functions": [
        {
          "type": "Function",
          "name": "add",
          "returnType": "int",
          "parameters": [
            {"name": "a", "type": "int"},
            {"name": "b", "type": "int"}
          ],
          "body": {
            "type": "Block",
            "statements": [
              {
                "type": "ReturnStatement",
                "expression": {
                  "type": "BinaryExpression",
                  "operator": "+",
                  "left": {"type": "Identifier", "name": "a"},
                  "right": {"type": "Identifier", "name": "b"}
                }
              }
            ]
          }
        },
        {
          "type": "Function",
          "name": "main",
          "returnType": "int",
          "parameters": [],
          "body": {
            "type": "Block",
            "statements": [
              {
                "type": "ReturnStatement",
                "expression": {"type": "Literal", "value": 0}
              }
            ]
          }
        }
      ]
    }
  }
}`;

export default function CodeGeneratorPage() {
  const [output, setOutput] = useState('');
  const [parsedOutput, setParsedOutput] = useState<Record<string, string> | null>(null);
  const [inputIsIR, setInputIsIR] = useState(false);
  const [currentInput, setCurrentInput] = useState(sampleCode);
  
  // Handle input toggle change
  useEffect(() => {
    // Switch between sample code and sample IR when the toggle changes
    setCurrentInput(inputIsIR ? sampleIR : sampleCode);
  }, [inputIsIR]);
  
  // Process the output whenever it changes
  useEffect(() => {
    if (!output) {
      setParsedOutput(null);
      return;
    }
    
    try {
      // Check if it's valid JSON with language outputs
      if (output.trim().startsWith('{') && output.trim().endsWith('}')) {
        const parsed = JSON.parse(output);
        
        // Check if it contains at least one language key
        const hasLanguageOutput = ['java', 'python', 'cpp', 'javascript'].some(
          lang => typeof parsed[lang] === 'string'
        );
        
        if (hasLanguageOutput) {
          setParsedOutput(parsed);
          return;
        }
      }
      
      // Not valid language outputs
      setParsedOutput(null);
    } catch (e) {
      console.error('Error parsing codegen output:', e);
      setParsedOutput(null);
    }
  }, [output]);
  
  const handleRunCodeGen = async (input: string) => {
    return runCodeGenerator(input, inputIsIR);
  };
  
  const renderOutput = () => {
    if (parsedOutput) {
      return <MultiCodeOutput output={output} title="Generated Code" />;
    } else {
      return (
        <OutputDisplay 
          output={output || "No output yet. Run the module to see results."} 
          title="Generated Code" 
          language="text" 
        />
      );
    }
  };
  
  return (
    <ModuleLayout
      title="Code Generator"
      description="The code generator transforms the IR into executable code in various target languages like C++, Java, JavaScript, and Python."
      defaultInput={currentInput}
      runModule={handleRunCodeGen}
      outputTitle="Generated Code"
      customOutputComponent={renderOutput}
      onOutputChange={setOutput}
      inputOptions={{
        showToggle: true,
        toggleLabel: "Input is IR",
        isToggled: inputIsIR,
        onToggleChange: setInputIsIR,
        toggleHelp: "When enabled, input is treated as IR. When disabled, input is treated as source code and will go through the full pipeline."
      }}
    />
  )
} 