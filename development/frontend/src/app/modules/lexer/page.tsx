'use client'

import { useState, useEffect } from 'react'
import ModuleLayout from '@/components/ModuleLayout';
import { executeModule } from '@/lib/api';
import { useEditor } from '@/lib/EditorContext';

export default function LexerPage() {
  const sampleCode = 'garden Example {\n  grow main() -> int {\n    bloom << "Hello, Hanami!";\n    blossom 0;\n  }\n}';
  const [output, setOutput] = useState<string>('');
  const [isLoading, setIsLoading] = useState<boolean>(false);
  const [error, setError] = useState<string | null>(null);
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
      console.log(`Running lexer with input: ${editorValue.substring(0, 50)}...`);
      const result = await executeModule('lexer', editorValue);
      setOutput(result);
      console.log('Lexer executed successfully');
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : 'An unknown error occurred';
      console.error('Error executing lexer:', err);
      setError(errorMessage);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <ModuleLayout 
      title="Lexer"
      description="The lexer breaks down source code into tokens."
      code={sampleCode}
      setCode={setEditorValue}
      output={output}
      isLoading={isLoading}
      error={error}
      onRun={handleRunModule}
      outputTitle="Tokens"
    />
  );
} 