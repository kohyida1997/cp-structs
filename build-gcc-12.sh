#!/bin/bash

echo -e "Building with gcc-12 if found\n"

# Check if build directory is set-up
if [ -d "build" ]; then
    [ ! -z "$(ls -A build)" ] && rm -r build/*
else 
    mkdir build
fi

# Set compilers if found
[ -f "/usr/bin/gcc-12" ] && export CC=/usr/bin/gcc-12 && echo "Found, Set gcc-12"
[ -f "/usr/bin/g++-12" ] && export CXX=/usr/bin/g++-12 && echo "Found, Set g++-12"

# Generate CMake files
cd build
echo -e "Running CMake ...\n"
cmake ..

# Actually Build
echo -e "Running Make (-j 6) ...\n"
make -j 6

# Done!
echo -e "Done making!"
