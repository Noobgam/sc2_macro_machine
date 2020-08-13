#!/bin/sh

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
#cmake .. -DCMAKE_BUILD_TYPE=Release
VERBOSE=1 cmake --build . --target main_bot
