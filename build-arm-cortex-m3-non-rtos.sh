#!/bin/bash
mkdir build-arm-cortex-m3-non-rtos
cd build-arm-cortex-m3-non-rtos
cmake -DCMAKE_C_COMPILER=/usr/bin/arm-none-eabi-gcc -DCMAKE_TOOLCHAIN_FILE=../ya_mem_arm_cortex_m3_non_rtos.cmake ..
make
