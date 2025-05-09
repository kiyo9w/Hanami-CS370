// Use a type for monaco to avoid direct imports during SSR
import type * as Monaco from 'monaco-editor';

// Ensure this code only runs in browser environment
export function registerHanamiLanguage() {
  // Return early if we're not in a browser environment
  if (typeof window === 'undefined') {
    return;
  }

  // Dynamically import monaco when needed
  const initMonaco = async () => {
    // Wait until monaco is available
    const monaco = (window as any).monaco;
    if (!monaco) return;

    try {
      // Register a new language
      monaco.languages.register({ id: 'hanami' });

      // Register a tokens provider for the language
      monaco.languages.setMonarchTokensProvider('hanami', {
        tokenizer: {
          root: [
            // Keywords
            [/\b(garden|species|hidden|open|grow|water|bloom|blossom|branch|else|while|for|in|auto|string|int|double|float|bool|char|void|true|false|null)\b/, 'keyword'],

            // Operators
            [/(\+|\-|\*|\/|\%|\=|\<|\>|\!|\&|\||\^|\~|\<<|\>>|\=\=|\!\=|\<\=|\>\=|\&\&|\|\||\+\+|\-\-|\+=|\-=|\*=|\/=|\%=)/, 'operator'],

            // Delimiters
            [/[\[\]\(\)\{\}\,\.\:\;]/, 'delimiter'],

            // Stream operators (specific to Hanami)
            [/\b(<<|>>)\b/, 'predefined'],

            // Function declarations
            [/\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\()/, 'function'],

            // Numbers
            [/\b\d+\.\d+\b/, 'number.float'],
            [/\b\d+\b/, 'number'],

            // Strings
            [/"([^"\\]|\\.)*$/, 'string.invalid'],  // Non-terminated string
            [/"/, { token: 'string.quote', bracket: '@open', next: '@string' }],

            // Comments
            [/\/\/.*$/, 'comment'],
            [/\/\*/, 'comment', '@comment'],

            // Identifiers
            [/[a-zA-Z_]\w*/, 'identifier'],
          ],

          comment: [
            [/[^\/*]+/, 'comment'],
            [/\/\*/, 'comment', '@push'],
            [/\*\//, 'comment', '@pop'],
            [/[\/*]/, 'comment']
          ],

          string: [
            [/[^\\"]+/, 'string'],
            [/\\./, 'string.escape'],
            [/"/, { token: 'string.quote', bracket: '@close', next: '@pop' }]
          ],
        }
      });

      // Define a new theme that contains rules for hanami token colors
      monaco.editor.defineTheme('hanami-dark', {
        base: 'vs-dark',
        inherit: true,
        rules: [
          { token: 'keyword', foreground: 'C586C0' },
          { token: 'operator', foreground: 'D4D4D4' },
          { token: 'delimiter', foreground: 'D4D4D4' },
          { token: 'predefined', foreground: '4EC9B0' },
          { token: 'function', foreground: 'DCDCAA' },
          { token: 'number', foreground: 'B5CEA8' },
          { token: 'number.float', foreground: 'B5CEA8' },
          { token: 'string', foreground: 'CE9178' },
          { token: 'string.quote', foreground: 'CE9178' },
          { token: 'string.escape', foreground: 'D7BA7D' },
          { token: 'comment', foreground: '6A9955' },
          { token: 'identifier', foreground: '9CDCFE' },
        ],
        colors: {}
      });

      // Register a completion item provider for hanami language
      monaco.languages.registerCompletionItemProvider('hanami', {
        provideCompletionItems: (model: Monaco.editor.ITextModel, position: Monaco.Position) => {
          // Create a word range to use for all completions
          const wordAtPosition = model.getWordUntilPosition(position);
          const range = {
            startLineNumber: position.lineNumber,
            endLineNumber: position.lineNumber,
            startColumn: wordAtPosition.startColumn,
            endColumn: wordAtPosition.endColumn
          };

          const suggestions = [
            {
              label: 'garden',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'garden ${1:name} {\n\t$0\n}',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Define a garden (similar to a namespace in C++).',
              range: range
            },
            {
              label: 'grow',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'grow ${1:functionName}(${2:params}) -> ${3:returnType} {\n\t$0\n}',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Define a function.',
              range: range
            },
            {
              label: 'bloom',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'bloom << ${1:expression};',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Output an expression.',
              range: range
            },
            {
              label: 'water',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'water >> ${1:variable};',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Input into a variable.',
              range: range
            },
            {
              label: 'blossom',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'blossom ${1:returnValue};',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Return a value from a function.',
              range: range
            },
            {
              label: 'branch',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'branch (${1:condition}) {\n\t$0\n}',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Conditional branch (if statement).',
              range: range
            },
            {
              label: 'else',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'else {\n\t$0\n}',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Else statement.',
              range: range
            },
            {
              label: 'while',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'while (${1:condition}) {\n\t$0\n}',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'While loop.',
              range: range
            },
            {
              label: 'int',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'int ${1:variableName} = ${2:0};',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Integer variable declaration.',
              range: range
            },
            {
              label: 'string',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'string ${1:variableName} = "${2:value}";',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'String variable declaration.',
              range: range
            },
            {
              label: 'double',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'double ${1:variableName} = ${2:0.0};',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Double variable declaration.',
              range: range
            },
            {
              label: 'bool',
              kind: monaco.languages.CompletionItemKind.Keyword,
              insertText: 'bool ${1:variableName} = ${2:true};',
              insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
              documentation: 'Boolean variable declaration.',
              range: range
            }
          ];
          return { suggestions };
        }
      });
    } catch (error) {
      console.error('Error registering Hanami language:', error);
    }
  };

  // Check if monaco is already loaded
  if ((window as any).monaco) {
    initMonaco();
  } else {
    // Wait for monaco to load
    window.addEventListener('monaco-load', initMonaco);
    // Fallback: check periodically for monaco
    const checkMonaco = setInterval(() => {
      if ((window as any).monaco) {
        clearInterval(checkMonaco);
        initMonaco();
      }
    }, 100);
    // Clear the interval after 10 seconds to prevent it from running forever
    setTimeout(() => clearInterval(checkMonaco), 10000);
  }
} 