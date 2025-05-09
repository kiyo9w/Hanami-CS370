'use client'

import ModuleLayout from '@/components/ModuleLayout'
import { runFullCompiler } from '@/lib/api'

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

export default function AllModulesPage() {
  return (
    <ModuleLayout
      title="Full Compilation Pipeline"
      description="Run the complete Hanami compiler pipeline: Lexical Analysis → Parsing → Semantic Analysis → Code Generation. This will process your Hanami code through all stages and generate the final output."
      defaultInput={sampleCode}
      runModule={runFullCompiler}
      outputLanguage="javascript"
      outputTitle="Generated Code"
    />
  )
} 