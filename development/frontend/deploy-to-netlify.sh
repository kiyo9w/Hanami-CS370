#!/bin/bash

# Hanami Frontend Netlify Deployment Script

echo "===== Hanami Frontend Deployment to Netlify ====="
echo "This script will help you deploy the Hanami playground to Netlify"
echo

# Check if npm is installed
if ! command -v npm &> /dev/null; then
    echo "Error: npm is not installed. Please install Node.js and npm first."
    exit 1
fi

# Install dependencies if they're not installed yet
if [ ! -d "node_modules" ]; then
    echo "Installing dependencies..."
    npm install
    echo
fi

# Build the project
echo "Building project for production..."
npm run build
echo

# Check if Netlify CLI is installed
if ! command -v netlify &> /dev/null; then
    echo "Netlify CLI not found. Installing..."
    npm install -g netlify-cli
    echo
fi

# Check if user is logged in to Netlify
echo "Checking Netlify authentication..."
if ! netlify status &> /dev/null; then
    echo "Please login to your Netlify account:"
    netlify login
    echo
fi

# Deploy to Netlify
echo "Deploying to Netlify..."
echo "You will be prompted to create a new site or select an existing one."
netlify deploy --prod --dir=out

echo
echo "===== Deployment Complete ====="
echo "You can now access your site at the URL provided above."
echo "Any future changes can be deployed again using this script." 