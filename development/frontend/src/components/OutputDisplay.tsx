'use client'

import { useState, useMemo } from 'react'
import { Prism as SyntaxHighlighter } from 'react-syntax-highlighter'
import { vscDarkPlus } from 'react-syntax-highlighter/dist/cjs/styles/prism'

interface OutputDisplayProps {
  output: string
  title?: string
  language?: string
}

export default function OutputDisplay({ 
  output, 
  title = 'Output', 
  language = 'json' 
}: OutputDisplayProps) {
  const [copied, setCopied] = useState(false)

  const handleCopy = () => {
    navigator.clipboard.writeText(output)
    setCopied(true)
    setTimeout(() => {
      setCopied(false)
    }, 2000)
  }

  // Process and format the output appropriately
  const processedOutput = useMemo(() => {
    if (!output) return 'No output yet. Run the module to see results.';
    
    // Handle the error message case
    if (output.startsWith('No output file generated')) {
      return output;
    }
    
    // If it's tokens output (which is not JSON), keep it as text
    if (title === 'Tokens' && !output.startsWith('{')) {
      // Use text syntax highlighting
      return output;
    }
    
    // For JSON output, try to pretty print it
    if (language === 'json') {
      try {
        const parsed = JSON.parse(output);
        return JSON.stringify(parsed, null, 2);
      } catch (e) {
        // If it's not valid JSON, return as is
        return output;
      }
    }
    
    return output;
  }, [output, language, title]);

  return (
    <div className="bg-gray-900 rounded-lg overflow-hidden">
      <div className="flex justify-between items-center px-4 py-2 bg-gray-800">
        <h3 className="text-sm font-medium text-white">{title}</h3>
        <button
          onClick={handleCopy}
          className="text-xs px-2 py-1 rounded bg-gray-700 text-gray-200 hover:bg-gray-600 transition-colors"
        >
          {copied ? 'Copied!' : 'Copy'}
        </button>
      </div>
      <div className="max-h-96 overflow-auto">
        <SyntaxHighlighter
          language={title === 'Tokens' && !output.startsWith('{') ? 'text' : language}
          style={vscDarkPlus}
          customStyle={{ margin: 0, padding: '1rem' }}
          wrapLines
          wrapLongLines
        >
          {processedOutput}
        </SyntaxHighlighter>
      </div>
    </div>
  )
} 