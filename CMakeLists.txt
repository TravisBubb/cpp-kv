cmake_minimum_required(VERSION 3.16)
project(KVStore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(KVStore 
    src/main.cpp
    src/in_memory_engine.cpp)
