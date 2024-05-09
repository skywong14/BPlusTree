cmake_minimum_required(VERSION 3.22)
project(BPlusTree)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories(.)

add_executable(code
        BPTree.hpp
        test.cpp
        vector.hpp
        exceptions.hpp)
