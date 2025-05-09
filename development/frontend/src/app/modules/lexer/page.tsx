'use client'

import { useEffect, useState } from 'react'
import ModuleLayout from '@/components/ModuleLayout'
import { runLexer } from '@/lib/api'

// Sample Hanami code for demo
const sampleCode = `
function main() {
  var x = 10;
  var y = 20;
  
  print("Hello, Hanami!");
  
  if (x < y) {
    print("x is less than y");
  }
  
  return 0;
}
`;

export default function LexerPage() {
  return (
    <ModuleLayout
      title="Lexical Analyzer (Lexer)"
      description="The lexer reads the source code as a stream of characters and converts it into meaningful tokens such as keywords, identifiers, operators, and literals."
      defaultInput={sampleCode}
      runModule={runLexer}
      outputLanguage="text"
      outputTitle="Tokens"
    />
  )
} 