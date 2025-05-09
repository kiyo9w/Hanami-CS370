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
  const [isRunning, setIsRunning] = useState(false);
  const [errorState, setError] = useState<string | null>(null);
  const [runCounter, setRunCounter] = useState<number>(0);
  
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

  // Update error state when the error prop changes
  useEffect(() => {
    setError(error);
  }, [error]);

  // Add a log entry when running or when errors occur
  const handleRun = async () => {
    try {
      setIsRunning(true);
      setEditorValue(editorValue); // Save any pending changes
      
      // Clear logs and increment run counter when running
      setLogs([]);
      setRunCounter(prev => prev + 1);
      setError(null); // Clear previous errors
      
      // Log which format is being used (helpful for troubleshooting)
      if (inputFormat) {
        setLogs(prev => [...prev, `Using input format: ${inputFormat}`]);
      }
      
      console.log(`Executing ${title} module with code: ${editorValue.substring(0, 50)}...`);
      
      await onRun();
      
      if (error) {
        console.error(`Error running ${title} module:`, error);
        setLogs(prev => [...prev, `ERROR: ${error}`]);
      } else {
        console.log(`${title} module executed successfully`);
        setLogs(prev => [...prev, `SUCCESS: ${title} execution completed.`]);
      }
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : 'Unknown error occurred';
      console.error(`Exception running ${title} module:`, err);
      setError(errorMessage);
      
      // Add detailed error to logs
      if (errorMessage.includes('Syntax Error:')) {
        setLogs(prev => [
          ...prev, 
          `ERROR: ${errorMessage}`
        ]);
      } else {
        setLogs(prev => [
          ...prev, 
          `ERROR: ${errorMessage}`,
          'Please make sure the backend server is running and accessible.'
        ]);
      }
      
      // For parser errors, add a hint
      if (title === 'Parser' && inputFormat === 'tokens') {
        setLogs(prev => [
          ...prev,
          'HINT: Try using "Source Code" format instead of "Tokens" format.'
        ]);
      }
      
      // For semantic analyzer errors, add a hint 
      if (title === 'Semantic Analyzer' && inputFormat === 'ast') {
        setLogs(prev => [
          ...prev,
          'HINT: Try using "Source Code" format instead of "AST" format.'
        ]);
      }
    } finally {
      setIsRunning(false);
    }
  };

  // Determine which output component to use
  const renderOutput = () => {
    // If we have a syntax error, display it prominently
    if (errorState && errorState.includes('Syntax Error:')) {
      return (
        <div className="bg-red-900/20 dark:bg-red-900/30 border border-red-700 rounded-lg p-4 text-red-500">
          <h3 className="font-bold mb-2 text-red-400">Syntax Error Detected</h3>
          <pre className="whitespace-pre-wrap font-mono text-sm">
            {errorState.replace('Syntax Error: ', '')}
          </pre>
          <p className="mt-3 text-sm text-red-400">
            Check your code for missing brackets, semicolons, or other syntax issues.
          </p>
        </div>
      );
    }
    
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
                  onChange={(e) => {
                    // Clear any errors when changing input format
                    setError(null);
                    onInputFormatChange(e.target.value);
                  }}
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
              disabled={isRunning}
              className="px-4 py-2.5 bg-gradient-to-r from-primary-600 to-primary-700 text-white rounded-md hover:from-primary-700 hover:to-primary-800 active:from-primary-800 active:to-primary-900 transition-colors disabled:opacity-50 disabled:cursor-not-allowed flex items-center font-medium shadow-md hover:shadow-lg"
            >
              {isRunning ? (
                <>
                  <svg className="animate-spin -ml-1 mr-2 h-4 w-4 text-white" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                    <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4"></circle>
                    <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
                  </svg>
                  Running...
                </>
              ) : (
                <>
                  <svg className="mr-1.5 h-5 w-5" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M14.752 11.168l-3.197-2.132A1 1 0 0010 9.87v4.263a1 1 0 001.555.832l3.197-2.132a1 1 0 000-1.664z"></path>
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M21 12a9 9 0 11-18 0 9 9 0 0118 0z"></path>
                  </svg>
                  Run Module
                </>
              )}
            </button>
            
            {errorState && !errorState.includes('Syntax Error:') && (
              <p className="text-red-500 text-sm px-3 py-2 rounded-md bg-red-50 dark:bg-red-900/20 border border-red-200 dark:border-red-800">
                Error: {errorState}
              </p>
            )}
          </div>
        </div>

        <div className="space-y-4">
          <h2 className="text-xl font-semibold">{outputTitle}</h2>
          {renderOutput()}
          
          <div>
            <h2 className="text-xl font-semibold mb-2">Terminal</h2>
            <Terminal 
              logs={logs} 
              enableLiveLogs={true} 
              moduleId={`${title}-${runCounter}`}
            />
          </div>
        </div>
      </div>
    </div>
  )
} 