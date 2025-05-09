'use client'

import { useState } from 'react'
import ModuleLayout from '@/components/ModuleLayout';
import { executeModule } from '@/lib/api';
import { useEditor } from '@/lib/EditorContext';

export default function AllModulesPage() {
  const [code, setCode] = useState<string>('garden Example {\n  grow main() -> int {\n    bloom << "Hello, Hanami!";\n    blossom 0;\n  }\n}');
  const [output, setOutput] = useState<string>('');
  const [isLoading, setIsLoading] = useState<boolean>(false);
  const [error, setError] = useState<string | null>(null);
  const { editorValue } = useEditor();

  const handleRunModule = async () => {
    setIsLoading(true);
    setError(null);
    
    try {
      const result = await executeModule('all', editorValue);
      setOutput(result);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'An unknown error occurred');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <ModuleLayout 
      title="Full Compilation Pipeline"
      description="Run the full Hanami compiler pipeline: lexer, parser, semantic analyzer, and code generator"
      code={code}
      setCode={setCode}
      output={output}
      isLoading={isLoading}
      error={error}
      onRun={handleRunModule}
      outputTitle="Generated Code"
      useMultiCodeOutput={true}
    />
  );
} 