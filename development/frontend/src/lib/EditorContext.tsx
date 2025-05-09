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
    'style <iostream>\nstyle <string>\n\ngarden SimpleGarden\n\nspecies CalAveragePoint {\nhidden:\n    float averagePoint;\n    float mathScore;\n    string title;\n    float EnglishScore;\n    \nopen: \n    grow TakingPoint() -> void {\n        bloom << "Enter math point: ";\n        water >> mathScore;\n        bloom << "Enter English point: ";\n        water >> EnglishScore;\n    }\n    \n    grow CalAveragePoint() -> float {\n        averagePoint = (mathScore + EnglishScore) / 2.0;\n        blossom averagePoint;\n    }\n    \n    grow printingTitle() -> string {\n        branch (averagePoint > 9.0) {\n            title = "Excellent";\n        } \n        else branch (averagePoint > 7.0) {\n            title = "Good";\n        } \n        else branch (averagePoint > 5.0) {\n            title = "Average";\n        } \n        else {\n            title = "Bad";\n        }\n        blossom title;\n    }\n};\n\ngrow main() -> int {\n    CalAveragePoint student;\n    student.TakingPoint();\n    \n    float avg = student.CalAveragePoint();\n    bloom << "Student average point is " << avg << "\\n";\n    bloom << "The title is " << student.printingTitle() << "\\n";\n    \n    blossom 0;\n}'
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