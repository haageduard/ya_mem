SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

ADD_DEFINITIONS(-DYA_MEM_ALLOC_POOL_SIZE=16)
ADD_DEFINITIONS(-DYA_MEM_ALLOC_CHUNK_SIZE=16)
ADD_DEFINITIONS(-DYA_MEM_ALLOC_USE_RTOS)
ADD_DEFINITIONS(-DYA_MEM_DEBUG)

SET(CMAKE_C_FLAGS "-mthumb -Og -g -std=c99 -fdata-sections -ffunction-sections -Wall -mcpu=cortex-m3 -specs=nano.specs" CACHE INTERNAL "c compiler flags")
SET(CMAKE_EXE_LINKER_FLAGS "-specs=nosys.specs -nostartfiles -static -Wl,-u,-Xlinker,-Map=output.map  -Wl,--gc-sections -Wl,--start-group -lc -lm -Wl,--end-group" CACHE INTERNAL "exe link flags")
