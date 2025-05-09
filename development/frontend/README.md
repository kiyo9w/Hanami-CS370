# Hanami Language Playground

An interactive web interface for exploring the Hanami programming language compiler modules - from lexical analysis to code generation.

## Features

- Interactive code editor with syntax highlighting
- Terminal output for debugging
- Separate modules for each compilation stage:
  - Lexical Analyzer (Lexer)
  - Parser
  - Semantic Analyzer
  - Code Generator
  - Full Compilation Pipeline

## Local Development

### Prerequisites

- Node.js 16.x or higher
- npm or yarn

### Setup

1. Clone the repository:
```bash
git clone https://github.com/kiyo9w/Hanami-CS370.git
cd Hanami-CS370/development/frontend
```

2. Install dependencies:
```bash
npm install
# or
yarn install
```

3. Start the development server:
```bash
npm run dev
# or
yarn dev
```

4. Open [http://localhost:3000](http://localhost:3000) in your browser to see the application.

## Building for Production

To create a production build:

```bash
npm run build
# or
yarn build
```

This will generate static files in the `out` directory that can be served by any static hosting service.

## Deployment

This project is configured for easy deployment to Netlify.

### Deploying to Netlify

1. Push your code to a Git repository (GitHub, GitLab, or Bitbucket).

2. Create a new site on Netlify:
   - Log in to [Netlify](https://app.netlify.com/)
   - Click "New site from Git"
   - Select your Git provider and repository
   - For build settings:
     - Build command: `npm run build`
     - Publish directory: `out`
   - Click "Deploy site"

3. Netlify will automatically build and deploy your site. Each time you push to the repository, Netlify will rebuild and redeploy.

## Project Structure

- `src/app/` - Page components and routing
- `src/components/` - Reusable React components
- `src/lib/` - Utility functions and API services
- `src/styles/` - Global styles and Tailwind configuration

## Technology Stack

- [Next.js](https://nextjs.org/) - React framework
- [TypeScript](https://www.typescriptlang.org/) - Type-safe JavaScript
- [Tailwind CSS](https://tailwindcss.com/) - Utility-first CSS framework
- [Monaco Editor](https://microsoft.github.io/monaco-editor/) - Code editor component
- [xterm.js](https://xtermjs.org/) - Terminal component

## License

This project is part of the Hanami CS370 course. See the main repository for license information.
