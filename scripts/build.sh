#!/bin/bash

# Build script for the 3D Black Hole Simulation

set -e  # Exit on any error

BUILD_TYPE=${1:-Release}
BUILD_DIR="build"

echo "🔨 Building Black Hole 3D Simulation"
echo "===================================="
echo "Build type: $BUILD_TYPE"

# Create build directory
if [ -d "$BUILD_DIR" ]; then
    echo "🧹 Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "⚙️  Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

echo "🔨 Building project..."
make -j$(nproc)

echo "✅ Build complete!"
echo "🎯 Executable: ./$BUILD_DIR/black_hole_3d"
