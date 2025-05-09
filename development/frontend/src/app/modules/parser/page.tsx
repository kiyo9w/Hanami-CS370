'use client'

import { useState } from 'react'
import ModuleLayout from '@/components/ModuleLayout'
import { runParser } from '@/lib/api'

// Sample tokens for the parser (from lexer output)
const sampleTokens = `GARDEN 1 1
IDENTIFIER Example 1 8
LEFT_BRACE 1 16
GROW 3 3
IDENTIFIER main 3 8
LEFT_PAREN 3 12
RIGHT_PAREN 3 13
ARROW 3 15
IDENTIFIER int 3 18
LEFT_BRACE 3 22
IDENTIFIER int 4 5
IDENTIFIER x 4 9
ASSIGN 4 11
NUMBER 10 4 13
SEMICOLON 4 15
BLOOM 6 5
STREAM_OUT 6 11
STRING The result is:  6 14
STREAM_OUT 6 31
IDENTIFIER x 6 34
STREAM_OUT 6 36
STRING \\n 6 39
SEMICOLON 6 43
BLOSSOM 8 5
NUMBER 0 8 13
SEMICOLON 8 14
RIGHT_BRACE 9 3
RIGHT_BRACE 10 1
EOF_TOKEN 10 2`;

export default function ParserPage() {
  const [inputIsTokens, setInputIsTokens] = useState(true);
  
  const handleRunParser = async (input: string) => {
    return runParser(input, inputIsTokens);
  };
  
  return (
    <ModuleLayout
      title="Parser"
      description="The parser analyzes tokens from the lexer and builds an Abstract Syntax Tree (AST) representing the program's structure. You can provide either source code (which will be lexed first) or tokens directly."
      defaultInput={sampleTokens}
      runModule={handleRunParser}
      outputLanguage="json"
      outputTitle="Abstract Syntax Tree (AST)"
      inputOptions={{
        showToggle: true,
        toggleLabel: "Input is tokens",
        isToggled: inputIsTokens,
        onToggleChange: setInputIsTokens,
        toggleHelp: "When enabled, input is treated as tokens. When disabled, input is treated as source code and will be lexed first."
      }}
    />
  )
} 