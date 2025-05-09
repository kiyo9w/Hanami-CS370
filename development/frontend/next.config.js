/** @type {import('next').NextConfig} */
const nextConfig = {
  reactStrictMode: true,
  images: {
    domains: ['localhost'],
    unoptimized: true,
  },
  output: 'export',
  trailingSlash: true,
  
  // Skip static generation for problematic pages
  skipTrailingSlashRedirect: true,
  skipMiddlewareUrlNormalize: true,
  
  // Redirect API calls to production backend
  async rewrites() {
    return [
      {
        source: '/api/:path*',
        destination: 'https://hanami-backend-production.up.railway.app/api/:path*',
      },
    ]
  },
  
  // Prevent server-side rendering for Monaco Editor
  webpack: (config, { isServer }) => {
    if (!isServer) {
      // Use external `monaco-editor` module only during client-side rendering
      config.resolve.fallback = {
        ...config.resolve.fallback,
        fs: false,
        path: false,
        os: false
      };
    }
    
    // Optimize chunks for dynamic imports
    config.optimization.splitChunks = {
      ...config.optimization.splitChunks,
      cacheGroups: {
        ...config.optimization.splitChunks.cacheGroups,
        monaco: {
          test: /[\\/]node_modules[\\/](monaco-editor)[\\/]/,
          name: 'monaco-editor',
          priority: 10,
          chunks: 'all'
        }
      }
    };
    
    // Add a null loader for Monaco during SSR to prevent 'self is not defined' errors
    if (isServer) {
      // Mock browser-specific modules on the server
      const originalEntry = config.entry;
      config.entry = async () => {
        const entries = await originalEntry();
        // Add polyfills for browser globals
        if (entries['main.js']) {
          entries['main.js'].unshift('./src/lib/browser-polyfills.js');
        }
        return entries;
      };
      
      // Avoid attempting to import Monaco on the server
      config.module.rules.push({
        test: /monaco-editor/,
        use: 'null-loader'
      });
    }
    
    return config;
  },
  // Ensure dynamic components load properly
  experimental: {
    optimizeCss: false, // Disable CSS optimization to avoid critters issues
    esmExternals: 'loose'
  }
}

module.exports = nextConfig 