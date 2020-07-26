#!/bin/sh

mkdir build
cd build
cmake ..
VERBOSE=1 cmake --build . --target meta_calculator
