'use client'

import { useState, useEffect } from 'react'
import { Prism as SyntaxHighlighter } from 'react-syntax-highlighter'
import { vscDarkPlus } from 'react-syntax-highlighter/dist/cjs/styles/prism'

interface MultiCodeOutputProps {
  output: string
  title?: string
}

// Language configurations with language name, syntax highlighting language, and tab label
const languageConfigs = {
  java: { name: 'Java', highlight: 'java', label: 'Java' },
  python: { name: 'Python', highlight: 'python', label: 'Python' },
  cpp: { name: 'C++', highlight: 'cpp', label: 'C++' },
  javascript: { name: 'JavaScript', highlight: 'javascript', label: 'JS' }
}

// Function to validate the output is a proper language outputs object
function isValidLanguageOutputs(obj: any): boolean {
  if (!obj || typeof obj !== 'object') return false;
  
  // Check that at least one language output exists
  const validLanguages = ['java', 'python', 'cpp', 'javascript'];
  return validLanguages.some(lang => typeof obj[lang] === 'string');
}

export default function MultiCodeOutput({ output, title = 'Generated Code' }: MultiCodeOutputProps) {
  const [activeTab, setActiveTab] = useState<string>('java')
  const [copied, setCopied] = useState(false)
  
  // Safe client-side navigator check
  const isClient = typeof window !== 'undefined'

  // Parse the output string (JSON) to an object
  let outputFiles: Record<string, string> = {}
  let availableTabs: string[] = []
  let parseError: string | null = null;

  try {
    if (output && output.trim()) {
      // If the output is a JSON string, try to parse it
      if (output.trim().startsWith('{') && output.trim().endsWith('}')) {
        const parsed = JSON.parse(output);
        
        if (isValidLanguageOutputs(parsed)) {
          outputFiles = parsed;
          availableTabs = Object.keys(outputFiles).filter(key => !!outputFiles[key]);
          
          // Set active tab to the first available language
          if (availableTabs.length > 0 && !outputFiles[activeTab]) {
            setActiveTab(availableTabs[0]);
          }
        } else {
          parseError = "Invalid language outputs format";
          if (isClient) console.error(parseError, parsed);
          outputFiles = { error: `The output doesn't contain valid language outputs: ${JSON.stringify(parsed)}` };
          availableTabs = ['error'];
        }
      } else {
        // If it's not JSON, put it in a default language (text)
        outputFiles = { text: output };
        availableTabs = ['text'];
      }
    }
  } catch (error) {
    parseError = `Error parsing output: ${error instanceof Error ? error.message : String(error)}`;
    if (isClient) console.error(parseError, output);
    outputFiles = { error: parseError };
    availableTabs = ['error'];
  }

  const handleCopy = () => {
    if (!isClient) return;
    
    const content = outputFiles[activeTab] || '';
    navigator.clipboard.writeText(content);
    setCopied(true);
    setTimeout(() => {
      setCopied(false);
    }, 2000);
  }

  // Show a placeholder if no output is available
  if (availableTabs.length === 0) {
    return (
      <div className="bg-gray-900 rounded-lg overflow-hidden">
        <div className="flex justify-between items-center px-4 py-2 bg-gray-800">
          <h3 className="text-sm font-medium text-white">{title}</h3>
        </div>
        <div className="p-4 text-gray-400">
          No code generated yet. Run the module to see results.
        </div>
      </div>
    );
  }

  // Get display name for the tab
  const getTabLabel = (tab: string): string => {
    const languageLabels: Record<string, string> = {
      java: 'Java',
      python: 'Python',
      cpp: 'C++',
      javascript: 'JS',
      text: 'Text',
      error: 'Error'
    };
    return languageLabels[tab] || tab;
  };

  // Get highlight language for syntax highlighting
  const getHighlightLanguage = (tab: string): string => {
    const highlightMap: Record<string, string> = {
      java: 'java',
      python: 'python',
      cpp: 'cpp',
      javascript: 'javascript',
      text: 'text',
      error: 'text'
    };
    return highlightMap[tab] || 'text';
  };

  return (
    <div className="bg-gray-900 rounded-lg overflow-hidden">
      {/* Header with tabs */}
      <div className="border-b border-gray-700">
        <div className="flex px-4 pt-2">
          {availableTabs.map((tab) => (
            <button
              key={tab}
              onClick={() => setActiveTab(tab)}
              className={`px-4 py-2 text-sm font-medium rounded-t-lg mr-2 ${
                activeTab === tab
                  ? 'bg-gray-800 text-primary-500 border-b-2 border-primary-500'
                  : 'text-gray-400 hover:text-gray-300 hover:bg-gray-800'
              }`}
            >
              {getTabLabel(tab)}
            </button>
          ))}
          <div className="flex-grow"></div>
          {isClient && (
            <button
              onClick={handleCopy}
              className="text-xs px-2 py-1 rounded bg-gray-700 text-gray-200 hover:bg-gray-600 transition-colors my-2"
            >
              {copied ? 'Copied!' : 'Copy'}
            </button>
          )}
        </div>
      </div>

      {/* Content area */}
      <div className="max-h-96 overflow-auto">
        <SyntaxHighlighter
          language={getHighlightLanguage(activeTab)}
          style={vscDarkPlus}
          customStyle={{ margin: 0, padding: '1rem' }}
          wrapLines
          wrapLongLines
        >
          {outputFiles[activeTab] || ''}
        </SyntaxHighlighter>
      </div>
    </div>
  )
} 