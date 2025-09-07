#!/bin/bash

# Exit on any error
set -e

echo "Building {{project_name}}..."

# Ensure we're in the project root directory
cd "$(dirname "$0")"

# Clean previous build if requested
if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    rm -rf out
fi

# Create build directory if it doesn't exist
mkdir -p out

# Configure the project with CMake
echo "Configuring CMake..."
cmake -S . -B out

# Build the project
echo "Building project..."
cd out
make -j$(nproc)
cd ..

echo "Build completed successfully!"
echo "Executable location: out/{{project_exec}}"

# Optionally run the executable if 'run' argument is passed
if [ "$1" = "run" ]; then
    echo "Running the application..."
    ./out/{{project_exec}}
fi


if [ "$1" = "test" ]; then
    echo "Running tests..."
    cd out
    ctest --output-on-failure
    cd ..
fi