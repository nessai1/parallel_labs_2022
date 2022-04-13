cmake_minimum_required(VERSION 3.21)
project(s2lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fopenmp)
set(CMAKE_C_COMPILER "/home/daniel/opt/usr/local/bin/mpicc")
set(CMAKE_CXX_COMPILER "/home/daniel/opt/usr/local/bin/mpic++")


add_executable(example example.cpp)
add_executable(task1 task1.cpp)
add_executable(task2 task2.cpp)
