#!/bin/bash

# Create a build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Change into the build directory
cd build

# Run CMake to generate build files
cmake -DCMAKE_BUILD_TYPE=$1 -S .. -B .

cmake --build . -j4
# Return to the original directory
cd ..