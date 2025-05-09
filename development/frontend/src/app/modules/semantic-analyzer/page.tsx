'use client'

import { useState } from 'react'
import ModuleLayout from '@/components/ModuleLayout'
import { runSemanticAnalyzer } from '@/lib/api'

// Sample AST for semantic analysis (output from parser)
const sampleAst = `{
  "type": "Program",
  "garden": {
    "name": "Example",
    "functions": [
      {
        "type": "Function",
        "name": "main",
        "returnType": "int",
        "parameters": [],
        "body": {
          "type": "Block",
          "statements": [
            {
              "type": "VariableDeclaration",
              "dataType": "int",
              "name": "x",
              "initializer": {
                "type": "Literal",
                "value": 10
              }
            },
            {
              "type": "OutputStatement",
              "expressions": [
                {
                  "type": "Literal",
                  "value": "The result is: "
                },
                {
                  "type": "Identifier",
                  "name": "x"
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
                "type": "Literal",
                "value": 0
              }
            }
          ]
        }
      }
    ]
  }
}`;

export default function SemanticAnalyzerPage() {
  const [inputIsAst, setInputIsAst] = useState(true);
  
  const handleRunSemanticAnalyzer = async (input: string) => {
    return runSemanticAnalyzer(input, inputIsAst);
  };
  
  return (
    <ModuleLayout
      title="Semantic Analyzer"
      description="The semantic analyzer examines the AST from the parser to check for semantic errors, build symbol tables, and perform type checking. You can provide either source code (which will go through lexing and parsing first) or an AST directly."
      defaultInput={sampleAst}
      runModule={handleRunSemanticAnalyzer}
      outputLanguage="json"
      outputTitle="Semantic Analysis Results"
      inputOptions={{
        showToggle: true,
        toggleLabel: "Input is AST",
        isToggled: inputIsAst,
        onToggleChange: setInputIsAst,
        toggleHelp: "When enabled, input is treated as an AST. When disabled, input is treated as source code and will go through lexing and parsing first."
      }}
    />
  )
} 