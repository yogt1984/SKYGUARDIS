#!/bin/bash
# Build script for SKYGUARDIS

set -e

echo "Building SKYGUARDIS..."

# Build C++ components
mkdir -p build
cd build
cmake ..
make -j$(nproc)

# Build Ada components
cd ..
gnatmake -P skyguardis.gpr

echo "Build complete!"

