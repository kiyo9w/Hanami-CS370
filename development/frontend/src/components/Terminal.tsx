'use client'

import { useEffect, useRef, useState } from 'react'
import { Terminal as XTerm } from 'xterm'
import { FitAddon } from 'xterm-addon-fit'
import { getTerminalLogs } from '@/lib/api'
import 'xterm/css/xterm.css'

interface TerminalProps {
  logs?: string[]
  enableLiveLogs?: boolean
}

export default function Terminal({ logs = [], enableLiveLogs = false }: TerminalProps) {
  const terminalRef = useRef<HTMLDivElement>(null)
  const xtermRef = useRef<XTerm | null>(null)
  const fitAddonRef = useRef<FitAddon | null>(null)
  const [liveLogs, setLiveLogs] = useState<string[]>([])

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

  useEffect(() => {
    if (!terminalRef.current) return

    // Initialize xterm.js
    const terminal = new XTerm({
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
    terminal.open(terminalRef.current)
    fitAddon.fit()

    xtermRef.current = terminal
    fitAddonRef.current = fitAddon

    // Clear and write welcome message
    terminal.clear()
    terminal.writeln('\x1B[1;34mWelcome to Hanami Terminal\x1B[0m')
    terminal.writeln('Run a module to see debug output here.\r\n')

    // Handle window resize
    const handleResize = () => {
      if (fitAddonRef.current) {
        fitAddonRef.current.fit()
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
    if (xtermRef.current && displayLogs.length > 0) {
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
  }, [displayLogs])

  return <div ref={terminalRef} className="xterm" />
} 