'use client'

import { useState } from 'react'
import ModuleLayout from '@/components/ModuleLayout';
import { executeModule } from '@/lib/api';
import { useEditor } from '@/lib/EditorContext';

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
  const [code, setCode] = useState<string>('garden Example {\n  grow main() -> int {\n    bloom << "Hello, Hanami!";\n    blossom 0;\n  }\n}');
  const [output, setOutput] = useState<string>('');
  const [isLoading, setIsLoading] = useState<boolean>(false);
  const [error, setError] = useState<string | null>(null);
  const [inputFormat, setInputFormat] = useState<'source' | 'ast'>('source');
  const { editorValue, setEditorValue } = useEditor();

  const handleRunModule = async () => {
    setIsLoading(true);
    setError(null);
    
    try {
      const result = await executeModule('semantic', editorValue, inputFormat);
      setOutput(result);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'An unknown error occurred');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <ModuleLayout 
      title="Semantic Analyzer"
      description="The semantic analyzer checks the AST for semantic correctness, ensuring variables are declared before use, type checking, and other semantic rules."
      code={code}
      setCode={setEditorValue}
      output={output}
      isLoading={isLoading}
      error={error}
      onRun={handleRunModule}
      outputTitle="Semantic Analysis Result"
      inputFormatOptions={[
        { value: 'source', label: 'Source Code' },
        { value: 'ast', label: 'AST' }
      ]}
      inputFormat={inputFormat}
      onInputFormatChange={setInputFormat}
    />
  );
} 