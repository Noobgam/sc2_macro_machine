#!/bin/sh

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
VERBOSE=1 cmake --build . --target meta_calculator
