cmake_minimum_required(VERSION 3.0)
project(muduo_in_cpp14 C CXX )

set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")
#[[set(CMAKE_CXX_COMPILER "g++")]]
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread   ")

enable_testing()
include_directories ("${PROJECT_SOURCE_DIR}/")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/)

add_subdirectory(muduo/base)
add_subdirectory(muduo/log)
add_subdirectory(muduo/net)
add_subdirectory(muduo/timer)
include_directories(${PROJECT_SOURCE_DIR})



