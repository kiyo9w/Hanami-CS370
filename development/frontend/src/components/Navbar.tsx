'use client'

import Link from 'next/link'
import { usePathname } from 'next/navigation'
import { useState } from 'react'

export default function Navbar() {
  const pathname = usePathname()
  const [mobileMenuOpen, setMobileMenuOpen] = useState(false)
  
  return (
    <nav className="bg-gradient-to-r from-gray-900 to-gray-800 text-white shadow-xl sticky top-0 z-50">
      <div className="container mx-auto px-4">
        <div className="flex items-center justify-between h-16">
          <div className="flex items-center">
            <Link href="/" className="flex items-center">
              <span className="text-2xl font-bold text-transparent bg-clip-text bg-gradient-to-r from-primary-400 to-primary-600 mr-2">
                Hanami
              </span>
              <span className="text-lg font-medium text-gray-300">Playground</span>
            </Link>
          </div>
          
          {/* Mobile menu button */}
          <div className="flex md:hidden">
            <button
              type="button"
              className="inline-flex items-center justify-center p-2 rounded-md text-gray-400 hover:text-white hover:bg-gray-700 focus:outline-none focus:ring-2 focus:ring-inset focus:ring-white"
              onClick={() => setMobileMenuOpen(!mobileMenuOpen)}
            >
              <span className="sr-only">Open main menu</span>
              {mobileMenuOpen ? (
                <svg className="block h-6 w-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M6 18L18 6M6 6l12 12" />
                </svg>
              ) : (
                <svg className="block h-6 w-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M4 6h16M4 12h16M4 18h16" />
                </svg>
              )}
            </button>
          </div>
          
          {/* Desktop navigation */}
          <div className="hidden md:block">
            <div className="ml-10 flex items-center space-x-3">
              <NavLink href="/" active={pathname === '/'}>
                <svg className="w-4 h-4 mr-1" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M3 12l2-2m0 0l7-7 7 7M5 10v10a1 1 0 001 1h3m10-11l2 2m-2-2v10a1 1 0 01-1 1h-3m-6 0a1 1 0 001-1v-4a1 1 0 011-1h2a1 1 0 011 1v4a1 1 0 001 1m-6 0h6" />
                </svg>
                Home
              </NavLink>
              <NavLink href="/modules/lexer" active={pathname === '/modules/lexer'}>Lexer</NavLink>
              <NavLink href="/modules/parser" active={pathname === '/modules/parser'}>Parser</NavLink>
              <NavLink href="/modules/semantic-analyzer" active={pathname === '/modules/semantic-analyzer'}>Semantic Analyzer</NavLink>
              <NavLink href="/modules/codegen" active={pathname === '/modules/codegen'}>Code Generator</NavLink>
              <NavLink href="/modules/all" active={pathname === '/modules/all'}>All Modules</NavLink>
              <NavLink href="https://github.com/kiyo9w/Hanami-CS370" external>
                <svg className="w-4 h-4 inline-block mr-1" fill="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
                  <path fillRule="evenodd" clipRule="evenodd" d="M12 2C6.477 2 2 6.477 2 12c0 4.42 2.87 8.17 6.84 9.5.5.08.66-.23.66-.5v-1.69c-2.77.6-3.36-1.34-3.36-1.34-.46-1.16-1.11-1.47-1.11-1.47-.91-.62.07-.6.07-.6 1 .07 1.53 1.03 1.53 1.03.87 1.52 2.34 1.07 2.91.83.09-.65.35-1.09.63-1.34-2.22-.25-4.55-1.11-4.55-4.92 0-1.11.38-2 1.03-2.71-.1-.25-.45-1.29.1-2.64 0 0 .84-.27 2.75 1.02.79-.22 1.65-.33 2.5-.33.85 0 1.71.11 2.5.33 1.91-1.29 2.75-1.02 2.75-1.02.55 1.35.2 2.39.1 2.64.65.71 1.03 1.6 1.03 2.71 0 3.82-2.34 4.66-4.57 4.91.36.31.69.92.69 1.85V21c0 .27.16.59.67.5C19.14 20.16 22 16.42 22 12A10 10 0 0012 2z" />
                </svg>
                GitHub
              </NavLink>
            </div>
          </div>
        </div>
        
        {/* Mobile menu, show/hide based on menu state */}
        <div className={`${mobileMenuOpen ? 'block' : 'hidden'} md:hidden bg-gray-800 rounded-b-lg shadow-lg`}>
          <div className="px-2 pt-2 pb-3 space-y-1 sm:px-3">
            <MobileNavLink href="/" active={pathname === '/'}>Home</MobileNavLink>
            <MobileNavLink href="/modules/lexer" active={pathname === '/modules/lexer'}>Lexer</MobileNavLink>
            <MobileNavLink href="/modules/parser" active={pathname === '/modules/parser'}>Parser</MobileNavLink>
            <MobileNavLink href="/modules/semantic-analyzer" active={pathname === '/modules/semantic-analyzer'}>Semantic Analyzer</MobileNavLink>
            <MobileNavLink href="/modules/codegen" active={pathname === '/modules/codegen'}>Code Generator</MobileNavLink>
            <MobileNavLink href="/modules/all" active={pathname === '/modules/all'}>All Modules</MobileNavLink>
            <MobileNavLink href="https://github.com/kiyo9w/Hanami-CS370" external>GitHub</MobileNavLink>
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
  const className = `px-3 py-2 rounded-md text-sm font-medium flex items-center ${
    active 
      ? 'bg-primary-600 text-white shadow-md' 
      : 'text-gray-300 hover:bg-gray-700 hover:text-white transition-colors'
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

function MobileNavLink({ 
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
  const className = `block px-3 py-2 rounded-md text-base font-medium ${
    active 
      ? 'bg-primary-600 text-white' 
      : 'text-gray-300 hover:bg-gray-700 hover:text-white'
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