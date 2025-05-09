'use client'

import { useEffect, useRef, useState } from 'react'
import dynamic from 'next/dynamic'

// Import these types but not the actual monaco instance
import type * as Monaco from 'monaco-editor'
import type { editor } from 'monaco-editor'

// Use dynamic import for Monaco editor
let monaco: typeof Monaco | null = null

// Generate unique IDs for editor instances
let editorCounter = 0;

// Safety check for SSR
const isBrowser = typeof window !== 'undefined';

// Separate component for Monaco editor that only renders on client side
function MonacoEditorClient({
  value,
  onChange,
  language,
  readOnly,
  id,
}: {
  value: string
  onChange: (value: string) => void
  language: string
  readOnly: boolean
  id: string
}) {
  const editorRef = useRef<HTMLDivElement>(null)
  const editorInstanceRef = useRef<editor.IStandaloneCodeEditor | null>(null)
  
  useEffect(() => {
    // Safety check for SSR
    if (!isBrowser) return;
    
    // Dynamically import Monaco and language support only on client
    const loadMonaco = async () => {
      if (typeof window === 'undefined') return
      
      try {
        // Dynamically import Monaco
        const monacoModule = await import('monaco-editor')
        monaco = monacoModule
        
        // Import language support
        const { registerHanamiLanguage } = await import('@/lib/hanami-language')
        registerHanamiLanguage()
        
        if (editorRef.current && monaco && !editorInstanceRef.current) {
          const editor = monaco.editor.create(editorRef.current, {
            value,
            language,
            theme: language === 'hanami' ? 'hanami-dark' : 'vs-dark',
            automaticLayout: true,
            minimap: { enabled: false },
            scrollBeyondLastLine: false,
            lineNumbers: 'on',
            readOnly,
            fontSize: 14,
            tabSize: 2,
            wordWrap: 'on'
          })

          editor.onDidChangeModelContent(() => {
            onChange(editor.getValue())
          })

          editorInstanceRef.current = editor
        }
      } catch (error) {
        console.error('Failed to load Monaco editor:', error);
      }
    }
    
    loadMonaco()
    
    // Cleanup function
    return () => {
      if (editorInstanceRef.current) {
        editorInstanceRef.current.dispose()
        editorInstanceRef.current = null
      }
    }
  }, [id]) // Add id to dependency array

  useEffect(() => {
    if (!editorInstanceRef.current) return;
    
    const currentValue = editorInstanceRef.current.getValue()
    if (currentValue !== value) {
      editorInstanceRef.current.setValue(value)
    }
  }, [value])

  return <div ref={editorRef} className="h-80 monaco-editor" id={id} />
}

// Fallback component for SSR
function EditorFallback() {
  return (
    <div className="h-80 border border-gray-700 bg-gray-900 rounded-lg flex items-center justify-center">
      <p className="text-gray-400">Loading editor...</p>
    </div>
  );
}

// Create a version of the editor that only renders on client
const DynamicMonacoEditor = dynamic(
  () => Promise.resolve(MonacoEditorClient),
  { 
    ssr: false,
    loading: () => <EditorFallback />
  }
)

interface CodeEditorProps {
  value: string
  onChange: (value: string) => void
  language?: string
  readOnly?: boolean
}

export default function CodeEditor({ 
  value, 
  onChange, 
  language = 'hanami',
  readOnly = false
}: CodeEditorProps) {
  // Don't render Monaco during SSR
  if (!isBrowser) {
    return <EditorFallback />;
  }
  
  // Create a unique ID for this editor instance
  const [editorId] = useState(() => `monaco-editor-${++editorCounter}`);
  
  return (
    <DynamicMonacoEditor
      value={value}
      onChange={onChange}
      language={language}
      readOnly={readOnly}
      id={editorId}
    />
  )
} 