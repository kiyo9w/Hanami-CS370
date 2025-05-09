'use client'

import { useEffect, useRef } from 'react'
import * as monaco from 'monaco-editor'
import { registerHanamiLanguage } from '@/lib/hanami-language'

// Register the Hanami language when this module is imported
// This is safe to call on the client side only
if (typeof window !== 'undefined') {
  registerHanamiLanguage()
}

interface CodeEditorProps {
  value: string
  onChange: (value: string) => void
  language?: string
  readOnly?: boolean
}

export default function CodeEditor({ 
  value, 
  onChange, 
  language = 'hanami',  // Default to our custom language
  readOnly = false
}: CodeEditorProps) {
  const editorRef = useRef<HTMLDivElement>(null)
  const editorInstanceRef = useRef<monaco.editor.IStandaloneCodeEditor | null>(null)

  useEffect(() => {
    if (editorRef.current) {
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

      return () => {
        editor.dispose()
      }
    }
  }, [])

  useEffect(() => {
    if (editorInstanceRef.current) {
      const currentValue = editorInstanceRef.current.getValue()
      if (currentValue !== value) {
        editorInstanceRef.current.setValue(value)
      }
    }
  }, [value])

  return <div ref={editorRef} className="monaco-editor" />
} 