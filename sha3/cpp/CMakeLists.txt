cmake_minimum_required(VERSION 2.8.4)
project(sha3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -std=c++11")

set(SOURCE_FILES sha3.cpp)
add_executable(sha3 ${SOURCE_FILES})
