#!/bin/bash

# Enhanced macOS Docker run script for 3D Black Hole Simulation
# Requires XQuartz for X11 forwarding

echo "🚀 Starting 3D Black Hole Simulation on macOS"
echo "============================================="

# Check if XQuartz is installed
if ! command -v xquartz &> /dev/null; then
    echo "⚠️  XQuartz not found. Please install it first:"
    echo "   brew install --cask xquartz"
    echo "   Then restart your Mac and run this script again."
    exit 1
fi

# Check if Docker is running
if ! docker info &> /dev/null; then
    echo "❌ Docker is not running. Please start Docker Desktop and try again."
    exit 1
fi

echo "📦 Building Docker image..."
docker build -t black-hole-3d .

if [ $? -ne 0 ]; then
    echo "❌ Docker build failed!"
    exit 1
fi

echo "🔧 Setting up X11 forwarding..."
# Allow X11 forwarding
xhost +localhost

echo "🎮 Starting simulation container..."
echo "📺 Connect to VNC at localhost:5900 to view the simulation"
echo "🛑 Press Ctrl+C to stop the container"

# Run container with proper settings for macOS
docker run -it --rm \
    --name black-hole-sim \
    --platform linux/arm64 \
    -p 5900:5900 \
    -v "$(pwd)/config:/app/config" \
    -v "$(pwd)/logs:/app/logs" \
    black-hole-3d

# Clean up X11 forwarding
echo "🧹 Cleaning up X11 forwarding..."
xhost -localhost

echo "✅ Simulation stopped."
