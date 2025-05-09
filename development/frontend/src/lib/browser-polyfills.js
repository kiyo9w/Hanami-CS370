// Polyfills for browser APIs in server-side contexts
if (typeof global !== 'undefined') {
  // Mock the 'self' object
  if (typeof self === 'undefined') {
    global.self = global;
  }
  
  // Mock the 'window' object
  if (typeof window === 'undefined') {
    global.window = global;
    
    // Add common window properties
    global.window.location = {
      href: 'https://localhost',
      host: 'localhost',
      hostname: 'localhost',
      pathname: '/',
      protocol: 'https:'
    };
    
    global.window.document = {
      createElement: () => ({}),
      getElementsByTagName: () => [],
      querySelector: () => null,
      querySelectorAll: () => [],
      getElementById: () => null,
      documentElement: {
        style: {}
      },
      head: {
        appendChild: () => {}
      },
      body: {
        appendChild: () => {}
      }
    };
    
    global.window.navigator = {
      userAgent: 'node',
      platform: process.platform,
      language: 'en-US'
    };
    
    global.window.addEventListener = () => {};
    global.window.removeEventListener = () => {};
    global.window.setTimeout = setTimeout;
    global.window.clearTimeout = clearTimeout;
    global.window.setInterval = setInterval;
    global.window.clearInterval = clearInterval;
    
    // Mock ResizeObserver
    global.window.ResizeObserver = class ResizeObserver {
      constructor(callback) {
        this.callback = callback;
      }
      observe() {}
      unobserve() {}
      disconnect() {}
    };
  }
  
  // Make common browser APIs available on the global scope
  if (typeof navigator === 'undefined') {
    global.navigator = {
      userAgent: 'node',
      platform: process.platform,
      language: 'en-US'
    };
  }
  
  // Mock document
  if (typeof document === 'undefined') {
    global.document = global.window?.document || {
      createElement: () => ({}),
      getElementsByTagName: () => [],
      querySelector: () => null,
      querySelectorAll: () => [],
      getElementById: () => null,
      documentElement: {
        style: {}
      },
      head: {
        appendChild: () => {}
      },
      body: {
        appendChild: () => {}
      }
    };
  }
} 