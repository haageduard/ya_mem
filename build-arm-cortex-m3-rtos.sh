#!/bin/bash
mkdir build-arm-cortex-m3-rtos
cd build-arm-cortex-m3-rtos
cmake -DCMAKE_C_COMPILER=/usr/bin/arm-none-eabi-gcc -DCMAKE_TOOLCHAIN_FILE=../ya_mem_arm_cortex_m3_rtos.cmake ..
make
