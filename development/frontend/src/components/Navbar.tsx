'use client'

import Link from 'next/link'
import { usePathname } from 'next/navigation'

export default function Navbar() {
  const pathname = usePathname()
  
  return (
    <nav className="bg-white dark:bg-gray-900 shadow-md">
      <div className="container mx-auto px-4">
        <div className="flex items-center justify-between h-16">
          <div className="flex items-center">
            <Link href="/" className="text-xl font-bold text-primary-600 dark:text-primary-400">
              Hanami Playground
            </Link>
          </div>
          
          <div className="hidden md:block">
            <div className="ml-10 flex items-baseline space-x-4">
              <NavLink href="/" active={pathname === '/'}>Home</NavLink>
              <NavLink href="/modules/lexer" active={pathname === '/modules/lexer'}>Lexer</NavLink>
              <NavLink href="/modules/parser" active={pathname === '/modules/parser'}>Parser</NavLink>
              <NavLink href="/modules/semantic-analyzer" active={pathname === '/modules/semantic-analyzer'}>Semantic Analyzer</NavLink>
              <NavLink href="/modules/codegen" active={pathname === '/modules/codegen'}>Code Generator</NavLink>
              <NavLink href="/modules/all" active={pathname === '/modules/all'}>All Modules</NavLink>
              <NavLink href="https://github.com/kiyo9w/Hanami-CS370" external>GitHub</NavLink>
            </div>
          </div>
        </div>
      </div>
    </nav>
  )
}

function NavLink({ 
  href, 
  active = false, 
  external = false,
  children 
}: { 
  href: string
  active?: boolean
  external?: boolean
  children: React.ReactNode 
}) {
  const className = `px-3 py-2 rounded-md text-sm font-medium ${
    active 
      ? 'bg-primary-100 text-primary-700 dark:bg-primary-900 dark:text-primary-300' 
      : 'text-gray-700 hover:bg-gray-100 dark:text-gray-300 dark:hover:bg-gray-800'
  }`
  
  if (external) {
    return (
      <a href={href} className={className} target="_blank" rel="noopener noreferrer">
        {children}
      </a>
    )
  }
  
  return (
    <Link href={href} className={className}>
      {children}
    </Link>
  )
} 