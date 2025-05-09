import Link from 'next/link'

export default function Home() {
  return (
    <div className="flex flex-col items-center justify-center min-h-[80vh] text-center">
      <h1 className="text-4xl font-bold mb-8">Hanami Language Playground</h1>
      <p className="text-xl mb-12 max-w-2xl">
        An interactive web interface for exploring the Hanami language compiler modules - from lexical analysis to code generation.
      </p>
      
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
        <ModuleCard 
          title="Lexer" 
          description="Transforms source code into a sequence of tokens"
          href="/modules/lexer"
        />
        <ModuleCard 
          title="Parser" 
          description="Builds an abstract syntax tree from tokens"
          href="/modules/parser"
        />
        <ModuleCard 
          title="Semantic Analyzer" 
          description="Performs semantic checks on the syntax tree"
          href="/modules/semantic-analyzer"
        />
        <ModuleCard 
          title="Code Generator" 
          description="Produces executable code from the syntax tree"
          href="/modules/codegen"
        />
        <ModuleCard 
          title="All Modules" 
          description="Run the complete compilation pipeline"
          href="/modules/all"
        />
      </div>
    </div>
  )
}

function ModuleCard({ title, description, href }: { title: string, description: string, href: string }) {
  return (
    <Link href={href} className="block">
      <div className="bg-white dark:bg-gray-800 p-6 rounded-lg shadow-md hover:shadow-lg transition-shadow border border-gray-200 dark:border-gray-700">
        <h2 className="text-2xl font-semibold mb-3">{title}</h2>
        <p className="text-gray-600 dark:text-gray-300">{description}</p>
      </div>
    </Link>
  )
}
