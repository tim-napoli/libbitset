cmake_minimum_required(VERSION 2.6)
project(bitset)

set(CMAKE_C_FLAGS "--std=c99 -Wall ${CMAKE_C_FLAGS}")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(bitset STATIC
            src/bitset.c)

