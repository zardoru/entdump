cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
project(entdump)
add_executable(entdump entdump.cpp print_lumps.cpp read_bsp.cpp)