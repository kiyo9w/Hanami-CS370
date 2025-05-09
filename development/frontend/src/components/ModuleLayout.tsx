'use client'

import { useState, ReactNode, useEffect } from 'react'
import OutputDisplay from './OutputDisplay'
import Terminal from './Terminal'
import MultiCodeOutput from './MultiCodeOutput'
import dynamic from 'next/dynamic'
import { useEditor } from '@/lib/EditorContext'

// Dynamically import CodeEditor with no SSR
const CodeEditor = dynamic(() => import('./CodeEditor'), {
  ssr: false,
  loading: () => <div className="h-80 border border-gray-700 bg-gray-900 rounded-lg flex items-center justify-center">
    <p className="text-gray-400">Loading editor...</p>
  </div>
});

interface InputFormatOption {
  value: string;
  label: string;
}

export interface ModuleLayoutProps {
  title: string;
  description: string;
  code: string;
  setCode: (code: string) => void;
  output: string;
  isLoading: boolean;
  error: string | null;
  onRun: () => Promise<void>;
  outputTitle?: string;
  inputFormat?: string;
  inputFormatOptions?: InputFormatOption[];
  onInputFormatChange?: (format: any) => void;
  useMultiCodeOutput?: boolean;
}

export default function ModuleLayout({
  title,
  description,
  code,
  setCode,
  output,
  isLoading,
  error,
  onRun,
  outputTitle = 'Output',
  inputFormat,
  inputFormatOptions,
  onInputFormatChange,
  useMultiCodeOutput = false
}: ModuleLayoutProps) {
  const [logs, setLogs] = useState<string[]>([]);
  
  // Connect to the global editor context
  const { editorValue, setEditorValue, editorKey, setModuleType } = useEditor();
  
  // Sync local state with global context
  useEffect(() => {
    // Initialize editor value from props
    if (code !== editorValue) {
      setEditorValue(code);
    }
    
    // Set the current module type
    setModuleType(title.toLowerCase());
  }, []);
  
  // Sync global state back to local props when needed
  useEffect(() => {
    if (editorValue !== code) {
      setCode(editorValue);
    }
  }, [editorValue]);

  // Add a log entry when running or when errors occur
  const handleRun = async () => {
    try {
      setLogs(prev => [...prev, 'Running module...']);
      await onRun();
      
      if (error) {
        setLogs(prev => [...prev, `ERROR: ${error}`]);
      } else {
        setLogs(prev => [...prev, 'Execution completed successfully.']);
      }
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : 'Unknown error occurred';
      setLogs(prev => [...prev, `ERROR: ${errorMessage}`]);
    }
  };

  // Determine which output component to use
  const renderOutput = () => {
    if (useMultiCodeOutput) {
      return <MultiCodeOutput output={output} title={outputTitle} />;
    } else {
      return (
        <OutputDisplay 
          output={output || "No output yet. Run the module to see results."} 
          title={outputTitle} 
          language="text" 
        />
      );
    }
  };

  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-3xl font-bold mb-2">{title}</h1>
        <p className="text-gray-600 dark:text-gray-300">{description}</p>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        <div className="space-y-4">
          <div className="flex justify-between items-center">
            <h2 className="text-xl font-semibold">Input</h2>
            
            {inputFormatOptions && inputFormatOptions.length > 0 && onInputFormatChange && (
              <div className="flex items-center space-x-2">
                <span className="text-sm">Input Format:</span>
                <select 
                  className="bg-gray-50 border border-gray-300 text-gray-900 text-sm rounded-lg focus:ring-primary-500 focus:border-primary-500 block p-2 dark:bg-gray-700 dark:border-gray-600 dark:text-white"
                  value={inputFormat}
                  onChange={(e) => onInputFormatChange(e.target.value)}
                >
                  {inputFormatOptions.map(option => (
                    <option key={option.value} value={option.value}>
                      {option.label}
                    </option>
                  ))}
                </select>
              </div>
            )}
          </div>
          
          <CodeEditor 
            key={editorKey}
            value={editorValue} 
            onChange={setEditorValue} 
            language="hanami"
          />
          
          <div className="flex items-center space-x-4">
            <button
              onClick={handleRun}
              disabled={isLoading}
              className="px-4 py-2 bg-primary-600 text-white rounded-md hover:bg-primary-700 transition-colors disabled:opacity-50 disabled:cursor-not-allowed"
            >
              {isLoading ? 'Running...' : 'Run Module'}
            </button>
          </div>
        </div>

        <div className="space-y-4">
          <h2 className="text-xl font-semibold">{outputTitle}</h2>
          {renderOutput()}
          
          <div>
            <h2 className="text-xl font-semibold mb-2">Terminal</h2>
            <Terminal logs={logs} enableLiveLogs={true} />
          </div>
        </div>
      </div>
    </div>
  )
} 