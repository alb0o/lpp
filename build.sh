#!/bin/bash
# Build script for LPP compiler

echo "Building LPP Compiler..."

# Create build directory
mkdir -p build
cd build

# Run CMake
cmake ..

# Build
cmake --build .

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful!"
    echo "Compiler executable: build/lppc"
    echo ""
    echo "To install: cd build && sudo make install"
else
    echo "Build failed!"
    exit 1
fi
