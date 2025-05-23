'use client'

import { useState, useEffect } from 'react'
import ModuleLayout from '@/components/ModuleLayout';
import { executeModule } from '@/lib/api';
import { useEditor } from '@/lib/EditorContext';

// Sample tokens for the parser (from lexer output)
const sampleTokens = `GARDEN 1 1
IDENTIFIER Example 1 8
LEFT_BRACE 1 16
GROW 3 3
IDENTIFIER main 3 8
LEFT_PAREN 3 12
RIGHT_PAREN 3 13
ARROW 3 15
IDENTIFIER int 3 18
LEFT_BRACE 3 22
IDENTIFIER int 4 5
IDENTIFIER x 4 9
ASSIGN 4 11
NUMBER 10 4 13
SEMICOLON 4 15
BLOOM 6 5
STREAM_OUT 6 11
STRING The result is:  6 14
STREAM_OUT 6 31
IDENTIFIER x 6 34
STREAM_OUT 6 36
STRING \\n 6 39
SEMICOLON 6 43
BLOSSOM 8 5
NUMBER 0 8 13
SEMICOLON 8 14
RIGHT_BRACE 9 3
RIGHT_BRACE 10 1
EOF_TOKEN 10 2`;

export default function ParserPage() {
  const sampleCode = 'style <iostream>\nstyle <string>\n\ngarden SimpleGarden\n\nspecies CalAveragePoint {\nhidden:\n    float averagePoint;\n    float mathScore;\n    string title;\n    float EnglishScore;\n    \nopen: \n    grow TakingPoint() -> void {\n        bloom << "Enter math point: ";\n        water >> mathScore;\n        bloom << "Enter English point: ";\n        water >> EnglishScore;\n    }\n    \n    grow CalAveragePoint() -> float {\n        averagePoint = (mathScore + EnglishScore) / 2.0;\n        blossom averagePoint;\n    }\n    \n    grow printingTitle() -> string {\n        branch (averagePoint > 9.0) {\n            title = "Excellent";\n        } \n        else branch (averagePoint > 7.0) {\n            title = "Good";\n        } \n        else branch (averagePoint > 5.0) {\n            title = "Average";\n        } \n        else {\n            title = "Bad";\n        }\n        blossom title;\n    }\n};\n\ngrow main() -> int {\n    CalAveragePoint student;\n    student.TakingPoint();\n    \n    float avg = student.CalAveragePoint();\n    bloom << "Student average point is " << avg << "\\n";\n    bloom << "The title is " << student.printingTitle() << "\\n";\n    \n    blossom 0;\n}';
  const [output, setOutput] = useState<string>('');
  const [isLoading, setIsLoading] = useState<boolean>(false);
  const [error, setError] = useState<string | null>(null);
  const [inputFormat, setInputFormat] = useState<'source' | 'tokens'>('source');
  const { editorValue, setEditorValue } = useEditor();
  
  // Set initial editor content
  useEffect(() => {
    if (!editorValue || editorValue.trim() === '') {
      setEditorValue(sampleCode);
    }
  }, [editorValue, setEditorValue]);

  const handleRunModule = async () => {
    if (!editorValue) {
      setError('No input provided.');
      return;
    }
    
    setIsLoading(true);
    setError(null);
    
    try {
      console.log(`Running parser with ${inputFormat} input: ${editorValue.substring(0, 50)}...`);
      const result = await executeModule('parser', editorValue, inputFormat);
      setOutput(result);
      console.log('Parser executed successfully');
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : 'An unknown error occurred';
      console.error('Error executing parser:', err);
      setError(errorMessage);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <ModuleLayout 
      title="Parser"
      description="The parser analyzes the structure of the tokens to create an Abstract Syntax Tree (AST)."
      code={sampleCode}
      setCode={setEditorValue}
      output={output}
      isLoading={isLoading}
      error={error}
      onRun={handleRunModule}
      outputTitle="Abstract Syntax Tree"
      inputFormatOptions={[
        { value: 'source', label: 'Source Code' },
        { value: 'tokens', label: 'Tokens' }
      ]}
      inputFormat={inputFormat}
      onInputFormatChange={setInputFormat}
    />
  );
} 