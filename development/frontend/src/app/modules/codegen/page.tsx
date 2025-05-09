'use client'

import { useState } from 'react'
import ModuleLayout from '@/components/ModuleLayout'
import { executeModule } from '@/lib/api'
import { useEditor } from '@/lib/EditorContext';

export default function CodegenPage() {
  const [code, setCode] = useState<string>('garden Example {\n  grow main() -> int {\n    bloom << "Hello, Hanami!";\n    blossom 0;\n  }\n}');
  const [output, setOutput] = useState<string>('');
  const [isLoading, setIsLoading] = useState<boolean>(false);
  const [error, setError] = useState<string | null>(null);
  const [inputFormat, setInputFormat] = useState<'source' | 'ir'>('source');
  const { editorValue } = useEditor();

  const handleRunModule = async () => {
    setIsLoading(true);
    setError(null);
    
    try {
      const result = await executeModule('codegen', editorValue, inputFormat);
      setOutput(result);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'An unknown error occurred');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <ModuleLayout 
      title="Code Generator"
      description="The code generator transforms the IR into target languages (Java, Python, C++, JavaScript)."
      code={code}
      setCode={setCode}
      output={output}
      isLoading={isLoading}
      error={error}
      onRun={handleRunModule}
      outputTitle="Generated Code"
      inputFormatOptions={[
        { value: 'source', label: 'Source Code' },
        { value: 'ir', label: 'IR' }
      ]}
      inputFormat={inputFormat}
      onInputFormatChange={setInputFormat}
      useMultiCodeOutput={true}
    />
  );
} 