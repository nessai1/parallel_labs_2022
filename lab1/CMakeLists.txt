cmake_minimum_required(VERSION 3.20)
project(Lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fopenmp)

add_executable(task_1 task_1.cpp)
add_executable(task_1_modified task_1_modified.cpp)
add_executable(task_2 task_2.cpp)