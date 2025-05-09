'use client'

import React, { createContext, useContext, useState, ReactNode } from 'react'

// Define the shape of the context
interface EditorContextType {
  editorValue: string
  setEditorValue: (value: string) => void
  editorKey: string
  moduleType: string
  setModuleType: (type: string) => void
}

// Create the context with default values
const EditorContext = createContext<EditorContextType>({
  editorValue: '',
  setEditorValue: () => {},
  editorKey: 'editor-primary',
  moduleType: 'none',
  setModuleType: () => {}
})

// Hook to use the editor context
export const useEditor = () => useContext(EditorContext)

// Provider component
export const EditorProvider = ({ children }: { children: ReactNode }) => {
  const [editorValue, setEditorValue] = useState<string>(
    'garden Example {\n  grow main() -> int {\n    bloom << "Hello, Hanami!";\n    blossom 0;\n  }\n}'
  )
  const [moduleType, setModuleType] = useState<string>('none')
  const editorKey = 'editor-primary' // Single constant key for the editor

  return (
    <EditorContext.Provider
      value={{
        editorValue,
        setEditorValue,
        editorKey,
        moduleType,
        setModuleType
      }}
    >
      {children}
    </EditorContext.Provider>
  )
} 