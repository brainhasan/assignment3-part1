#!/bin/bash
# Run unit tests for the assignment

# Automate these steps from the readme:
# Create a build subdirectory, change into it, run
# cmake .. && make && run the assignment-autotest application



mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .
make clean
make
cd ..
./build/assignment-autotest/assignment-autotest
