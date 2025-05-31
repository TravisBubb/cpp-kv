cmake_minimum_required(VERSION 3.16)
project(KVStore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0135 NEW)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)

# Fetch dependencies
include(cmake/FetchGRPC.cmake)
include(cmake/FetchGoogleTest.cmake)

# Enable testing
include(CTest)
enable_testing()

# Subdirectories
add_subdirectory(proto)
add_subdirectory(libs/common)
add_subdirectory(libs/storage)
add_subdirectory(apps/storage-node)
add_subdirectory(tests)

