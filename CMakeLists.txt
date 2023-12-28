cmake_minimum_required(VERSION 3.26)
project(evolutivos C)

set(CMAKE_C_STANDARD 11)

add_executable(evolutivos ag_1.c)

# Link against the math library
target_link_libraries(evolutivos m)