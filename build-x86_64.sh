#!/bin/bash
mkdir build-x86_64
cd build-x86_64
cmake -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_TOOLCHAIN_FILE=../ya_mem_x86_64.cmake ..
make
