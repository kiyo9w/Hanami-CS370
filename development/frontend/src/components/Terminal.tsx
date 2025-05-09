'use client'

import { useEffect, useRef, useState } from 'react'
import dynamic from 'next/dynamic'
import { getTerminalLogs } from '@/lib/api'
// Import the xterm CSS directly
import 'xterm/css/xterm.css'

// Define the TerminalProps interface
interface TerminalProps {
  logs?: string[]
  enableLiveLogs?: boolean
}

// Create a Terminal fallback for SSR
const TerminalFallback = () => (
  <div className="xterm bg-[#1e1e1e] p-4 rounded-lg h-[300px] flex items-center justify-center">
    <p className="text-gray-400">Loading terminal...</p>
  </div>
);

// Client-side Terminal implementation
const TerminalClient = ({ logs = [], enableLiveLogs = false }: TerminalProps) => {
  const terminalRef = useRef<HTMLDivElement>(null)
  const xtermRef = useRef<any>(null)
  const fitAddonRef = useRef<any>(null)
  const [liveLogs, setLiveLogs] = useState<string[]>([])
  const [loaded, setLoaded] = useState(false)

  // Fetch logs from backend periodically if enabled
  useEffect(() => {
    if (!enableLiveLogs) return
    
    // Fetch logs immediately
    fetchLogs()
    
    // Then fetch every 3 seconds
    const interval = setInterval(fetchLogs, 3000)
    
    return () => {
      clearInterval(interval)
    }
  }, [enableLiveLogs])
  
  const fetchLogs = async () => {
    try {
      const newLogs = await getTerminalLogs()
      setLiveLogs(newLogs)
    } catch (error) {
      console.error('Failed to fetch logs:', error)
    }
  }

  // Initialize xterm.js
  useEffect(() => {
    if (!terminalRef.current) return
    
    // Use dynamic imports to load xterm.js modules
    const initTerminal = async () => {
      try {
        // Import xterm modules dynamically
        const { Terminal } = await import('xterm')
        const { FitAddon } = await import('xterm-addon-fit')
        
        // Create terminal instance
        const terminal = new Terminal({
          theme: {
            background: '#1e1e1e',
            foreground: '#f8f8f8',
            cursor: '#f8f8f8'
          },
          cursorBlink: true,
          fontSize: 14,
          fontFamily: 'Menlo, Monaco, "Courier New", monospace',
          convertEol: true,
        })

        const fitAddon = new FitAddon()
        terminal.loadAddon(fitAddon)
        
        // Ensure the DOM element exists before opening the terminal
        if (terminalRef.current) {
          // Open terminal in the DOM element
          terminal.open(terminalRef.current)
          
          // Add a small delay to ensure terminal is rendered
          setTimeout(() => {
            try {
              fitAddon.fit()
            } catch (e) {
              console.warn('Failed to fit terminal', e)
            }
          }, 100)

          xtermRef.current = terminal
          fitAddonRef.current = fitAddon

          // Clear and write welcome message
          terminal.clear()
          terminal.writeln('\x1B[1;34mWelcome to Hanami Terminal\x1B[0m')
          terminal.writeln('Run a module to see debug output here.\r\n')
          
          setLoaded(true)
        }
      } catch (error) {
        console.error('Failed to initialize terminal:', error)
      }
    }

    initTerminal()

    // Handle window resize
    const handleResize = () => {
      if (fitAddonRef.current) {
        try {
          fitAddonRef.current.fit()
        } catch (e) {
          console.warn('Resize error:', e)
        }
      }
    }

    window.addEventListener('resize', handleResize)

    return () => {
      window.removeEventListener('resize', handleResize)
      if (xtermRef.current) {
        xtermRef.current.dispose()
      }
    }
  }, [])

  // Use either provided logs or live logs from backend
  const displayLogs = enableLiveLogs ? liveLogs : logs

  // Update terminal when logs change
  useEffect(() => {
    if (xtermRef.current && loaded && displayLogs.length > 0) {
      xtermRef.current.clear()
      
      displayLogs.forEach(log => {
        if (log.startsWith('ERROR:')) {
          xtermRef.current?.writeln(`\x1B[1;31m${log}\x1B[0m`)
        } else if (log.startsWith('WARNING:')) {
          xtermRef.current?.writeln(`\x1B[1;33m${log}\x1B[0m`)
        } else if (log.startsWith('SUCCESS:')) {
          xtermRef.current?.writeln(`\x1B[1;32m${log}\x1B[0m`)
        } else {
          xtermRef.current?.writeln(log)
        }
      })
    }
  }, [displayLogs, loaded])

  return <div ref={terminalRef} className="xterm" />
}

// Export a dynamic component with SSR disabled
export default dynamic(() => Promise.resolve(TerminalClient), {
  ssr: false,
  loading: () => <TerminalFallback />
}) 