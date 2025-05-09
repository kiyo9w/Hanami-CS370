'use client'

import { useEffect, useState, useRef } from 'react'
import { getTerminalLogs } from '@/lib/api'

// Define the TerminalProps interface
interface TerminalProps {
  logs?: string[]
  enableLiveLogs?: boolean
  moduleId?: string // To detect module changes
}

export default function Terminal({ logs = [], enableLiveLogs = false, moduleId }: TerminalProps) {
  const [terminalLogs, setTerminalLogs] = useState<string[]>(logs || [])
  const [error, setError] = useState<string | null>(null)
  const terminalRef = useRef<HTMLDivElement>(null)
  const contentRef = useRef<HTMLDivElement>(null)
  
  // Clear logs when moduleId changes (user switches modules)
  useEffect(() => {
    setTerminalLogs([])
    setError(null)
  }, [moduleId])
  
  // Fetch logs from backend
  useEffect(() => {
    if (!enableLiveLogs) return
    
    // Initial fetch
    fetchAndUpdateLogs()
    
    // Set up polling interval for live logs
    const interval = setInterval(fetchAndUpdateLogs, 2000)
    
    return () => clearInterval(interval)
  }, [enableLiveLogs])
  
  // When direct logs prop changes, update them (clear previous logs first)
  useEffect(() => {
    if (logs && logs.length > 0) {
      setTerminalLogs(logs)
    }
  }, [logs])
  
  // Scroll to bottom whenever logs change
  useEffect(() => {
    if (contentRef.current) {
      contentRef.current.scrollTop = contentRef.current.scrollHeight
    }
  }, [terminalLogs])
  
  // Function to fetch logs from backend
  const fetchAndUpdateLogs = async () => {
    try {
      const backendLogs = await getTerminalLogs()
      if (backendLogs && backendLogs.length > 0) {
        setTerminalLogs(backendLogs)
      }
    } catch (err) {
      console.error('Error fetching logs:', err)
      setError(err instanceof Error ? err.message : 'Error fetching logs')
    }
  }
  
  // Format log based on content
  const formatLog = (log: string) => {
    // Check for different types of syntax errors
    if (log.includes('Syntax Error:') || 
        log.includes('Parse error') || 
        log.includes('Error at') || 
        log.includes('[Line') ||
        (log.includes('ERROR:') && (log.includes('missing') || log.includes('unexpected')))) {
      return (
        <div className="p-1 bg-red-900/30 border-l-2 border-red-500 rounded my-1">
          <span className="text-red-500 font-bold">SYNTAX ERROR: </span>
          <span className="text-red-400">{log.replace('ERROR: Syntax Error:', '').replace('ERROR:', '')}</span>
        </div>
      );
    }
    else if (log.includes('ERROR:')) {
      return <span className="text-red-500">{log}</span>
    } else if (log.includes('SUCCESS:')) {
      return <span className="text-green-500">{log}</span>
    } else if (log.includes('WARNING:') || log.includes('HINT:')) {
      return <span className="text-yellow-500">{log}</span>
    } else if (log.includes('DEBUG:')) {
      return <span className="text-blue-400">{log}</span>
    } else if (log.startsWith('$') || log.startsWith('>')) {
      // Command style
      return (
        <>
          <span className="text-green-400">{log.substring(0, 1)}</span>
          <span className="text-white">{log.substring(1)}</span>
        </>
      )
    }
    return log
  }
  
  return (
    <div 
      ref={terminalRef}
      className="bg-black text-gray-200 rounded-lg h-[300px] overflow-hidden font-mono text-sm border border-gray-700 flex flex-col"
      style={{ boxShadow: '0 0 10px rgba(0, 0, 0, 0.5)', fontFamily: "'JetBrains Mono', 'Courier New', monospace" }}
    >
      {/* Fixed terminal header */}
      <div className="flex items-center p-2 border-b border-gray-800 bg-zinc-900 sticky top-0 z-10">
        <div className="flex gap-2 mr-2">
          <div className="w-3 h-3 rounded-full bg-red-500"></div>
          <div className="w-3 h-3 rounded-full bg-yellow-500"></div>
          <div className="w-3 h-3 rounded-full bg-green-500"></div>
        </div>
        <div className="text-center flex-grow text-gray-400 text-xs font-semibold">
          hanami@compiler ~ terminal
        </div>
      </div>
      
      {/* Scrollable content area */}
      <div ref={contentRef} className="flex-1 overflow-auto p-3">
        {error && (
          <div className="text-red-500 mb-2">
            <span className="text-red-400">Error:</span> {error}
          </div>
        )}
        
        {terminalLogs.length === 0 ? (
          <div className="flex flex-col">
            <span className="text-green-400">hanami@compiler:~$</span>
            <span className="text-gray-500 mt-1">Ready. Run a module to see output.</span>
            <span className="text-gray-500 text-xs mt-3 blink">_</span>
          </div>
        ) : (
          <div>
            <div className="text-green-400 mb-2">hanami@compiler:~$ run_module</div>
            {terminalLogs.map((log, index) => (
              <div key={index} className="whitespace-pre-wrap mb-1">
                {formatLog(log)}
              </div>
            ))}
            <span className="text-gray-500 text-xs blink">_</span>
          </div>
        )}
      </div>

      <style jsx>{`
        .blink {
          animation: blink-animation 1s steps(2, start) infinite;
        }
        @keyframes blink-animation {
          to {
            visibility: hidden;
          }
        }
      `}</style>
    </div>
  )
} 