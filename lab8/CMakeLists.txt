cmake_minimum_required(VERSION 3.21)
project(lab8)

include_directories(/home/daniel/opt/usr/local/include)

set(CMAKE_C_COMPILER "/home/daniel/opt/usr/local/bin/mpicc")
set(CMAKE_CXX_COMPILER "/home/daniel/opt/usr/local/bin/mpic++")
set(CMAKE_CXX_STANDARD 17)

add_executable(task_1 task_1.cpp)

add_executable(tst tst.cpp)