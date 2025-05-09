'use client'

import { useState } from 'react'
import CodeEditor from './CodeEditor'
import Terminal from './Terminal'
import OutputDisplay from './OutputDisplay'
import ClientOnly from './ClientOnly'

interface InputOptions {
  showToggle: boolean;
  toggleLabel: string;
  isToggled: boolean;
  onToggleChange: (value: boolean) => void;
  toggleHelp?: string;
}

interface ModuleLayoutProps {
  title: string
  description: string
  defaultInput: string
  runModule: (input: string) => Promise<{ output: string, logs: string[] }>
  outputLanguage?: string
  outputTitle?: string
  customOutputComponent?: () => JSX.Element
  onOutputChange?: (output: string) => void
  inputOptions?: InputOptions
}

export default function ModuleLayout({
  title,
  description,
  defaultInput,
  runModule,
  outputLanguage = 'json',
  outputTitle = 'Output',
  customOutputComponent,
  onOutputChange,
  inputOptions
}: ModuleLayoutProps) {
  const [input, setInput] = useState(defaultInput)
  const [output, setOutput] = useState('')
  const [logs, setLogs] = useState<string[]>([])
  const [loading, setLoading] = useState(false)
  const [enableLiveLogs, setEnableLiveLogs] = useState(true)

  const handleRun = async () => {
    setLoading(true)
    try {
      setLogs(['Running module...'])
      const result = await runModule(input)
      setOutput(result.output)
      setLogs(result.logs)
      
      // Call onOutputChange callback if provided
      if (onOutputChange) {
        onOutputChange(result.output);
      }
    } catch (error) {
      console.error(error)
      setLogs([`ERROR: ${error instanceof Error ? error.message : 'Unknown error occurred'}`])
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-3xl font-bold mb-2">{title}</h1>
        <p className="text-gray-600 dark:text-gray-300">{description}</p>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        <div className="space-y-4">
          <div className="flex justify-between items-center">
            <h2 className="text-xl font-semibold">Input</h2>
            {inputOptions?.showToggle && (
              <div className="flex items-center">
                <label className="inline-flex items-center cursor-pointer">
                  <input
                    type="checkbox"
                    checked={inputOptions.isToggled}
                    onChange={(e) => inputOptions.onToggleChange(e.target.checked)}
                    className="sr-only peer"
                  />
                  <div className="relative w-11 h-6 bg-gray-200 peer-focus:outline-none peer-focus:ring-4 peer-focus:ring-primary-300 dark:peer-focus:ring-primary-800 rounded-full peer dark:bg-gray-700 peer-checked:after:translate-x-full rtl:peer-checked:after:-translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:start-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all dark:border-gray-600 peer-checked:bg-primary-600"></div>
                  <span className="ms-3 text-sm font-medium text-gray-900 dark:text-gray-300">
                    {inputOptions.toggleLabel}
                  </span>
                </label>
                {inputOptions.toggleHelp && (
                  <div className="ml-2 text-gray-500 dark:text-gray-400 text-xs">
                    <span title={inputOptions.toggleHelp}>ⓘ</span>
                  </div>
                )}
              </div>
            )}
          </div>
          <ClientOnly>
            <CodeEditor 
              value={input} 
              onChange={setInput} 
              language="hanami"
            />
          </ClientOnly>
          <div className="flex items-center space-x-4">
            <button
              onClick={handleRun}
              disabled={loading}
              className="px-4 py-2 bg-primary-600 text-white rounded-md hover:bg-primary-700 transition-colors disabled:opacity-50 disabled:cursor-not-allowed"
            >
              {loading ? 'Running...' : 'Run Module'}
            </button>
            <div className="flex items-center">
              <input
                type="checkbox"
                id="enableLiveLogs"
                checked={enableLiveLogs}
                onChange={(e) => setEnableLiveLogs(e.target.checked)}
                className="mr-2"
              />
              <label htmlFor="enableLiveLogs" className="text-sm">Enable live logs</label>
            </div>
          </div>
        </div>

        <div className="space-y-4">
          <h2 className="text-xl font-semibold">{outputTitle}</h2>
          {customOutputComponent ? (
            customOutputComponent()
          ) : (
            <OutputDisplay 
              output={output} 
              title={outputTitle}
              language={outputLanguage}
            />
          )}
          <div>
            <h2 className="text-xl font-semibold mb-2">Terminal</h2>
            <ClientOnly>
              <Terminal logs={logs} enableLiveLogs={enableLiveLogs} />
            </ClientOnly>
          </div>
        </div>
      </div>
    </div>
  )
} 