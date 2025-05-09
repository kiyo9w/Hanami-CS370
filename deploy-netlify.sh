#!/bin/bash

# Netlify deployment script for Hanami frontend

echo "=== Hanami Frontend Deployment to Netlify ==="
echo "Make sure you are logged in to Netlify CLI"

# Check if Netlify CLI is installed
if ! command -v netlify &> /dev/null; then
    echo "Netlify CLI not found. Installing..."
    npm install -g netlify-cli
fi

# Verify login status
netlify status || netlify login

# Navigate to the frontend directory
cd development/frontend

# Install dependencies
echo "Installing dependencies..."
npm install

# Build the project with production API URL
echo "Building project..."
NEXT_PUBLIC_API_URL="https://hanami-backend-production.up.railway.app/api" npm run build

# Deploy to Netlify
echo "Deploying to Netlify..."
netlify deploy --prod

echo "=== Deployment Complete ==="
echo "Your frontend should be available at: https://hanami.netlify.app" 