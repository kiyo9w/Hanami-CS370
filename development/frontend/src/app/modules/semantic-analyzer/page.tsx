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

const sampleCode = 'style <iostream>\nstyle <string>\n\ngarden SimpleGarden\n\nspecies CalAveragePoint {\nhidden:\n    float averagePoint;\n    float mathScore;\n    string title;\n    float EnglishScore;\n    \nopen: \n    grow TakingPoint() -> void {\n        bloom << "Enter math point: ";\n        water >> mathScore;\n        bloom << "Enter English point: ";\n        water >> EnglishScore;\n    }\n    \n    grow CalAveragePoint() -> float {\n        averagePoint = (mathScore + EnglishScore) / 2.0;\n        blossom averagePoint;\n    }\n    \n    grow printingTitle() -> string {\n        branch (averagePoint > 9.0) {\n            title = "Excellent";\n        } \n        else branch (averagePoint > 7.0) {\n            title = "Good";\n        } \n        else branch (averagePoint > 5.0) {\n            title = "Average";\n        } \n        else {\n            title = "Bad";\n        }\n        blossom title;\n    }\n};\n\ngrow main() -> int {\n    CalAveragePoint student;\n    student.TakingPoint();\n    \n    float avg = student.CalAveragePoint();\n    bloom << "Student average point is " << avg << "\\n";\n    bloom << "The title is " << student.printingTitle() << "\\n";\n    \n    blossom 0;\n}';

export default function SemanticAnalyzerPage() {
  const [code, setCode] = useState<string>('style <iostream>\nstyle <string>\n\ngarden SimpleGarden\n\nspecies CalAveragePoint {\nhidden:\n    float averagePoint;\n    float mathScore;\n    string title;\n    float EnglishScore;\n    \nopen: \n    grow TakingPoint() -> void {\n        bloom << "Enter math point: ";\n        water >> mathScore;\n        bloom << "Enter English point: ";\n        water >> EnglishScore;\n    }\n    \n    grow CalAveragePoint() -> float {\n        averagePoint = (mathScore + EnglishScore) / 2.0;\n        blossom averagePoint;\n    }\n    \n    grow printingTitle() -> string {\n        branch (averagePoint > 9.0) {\n            title = "Excellent";\n        } \n        else branch (averagePoint > 7.0) {\n            title = "Good";\n        } \n        else branch (averagePoint > 5.0) {\n            title = "Average";\n        } \n        else {\n            title = "Bad";\n        }\n        blossom title;\n    }\n};\n\ngrow main() -> int {\n    CalAveragePoint student;\n    student.TakingPoint();\n    \n    float avg = student.CalAveragePoint();\n    bloom << "Student average point is " << avg << "\\n";\n    bloom << "The title is " << student.printingTitle() << "\\n";\n    \n    blossom 0;\n}');
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