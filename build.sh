#!/bin/sh

rm -rf build
mkdir build
cd build
cmake ..
VERBOSE=1 cmake --build .

