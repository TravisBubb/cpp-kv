cmake_minimum_required(VERSION 3.16)
project(KVStore, LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Gather src files, excluding main.cpp ===
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(FILTER SRC_FILES EXCLUDE REGEX ".*/main\\.cpp$") 

# === Build the core library ===
add_library(core STATIC ${SRC_FILES})
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic -g)

# === Build the main application ===
add_executable(app ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(app PRIVATE core)
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic -g)

# === GoogleTest setup ===
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL         https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# === Build the test runner ===
file(GLOB TEST_FILES ${CMAKE_SOURCE_DIR}/tests/*.cpp)
add_executable(test_suite ${TEST_FILES})
target_link_libraries(test_suite PRIVATE core gtest_main)
add_test(NAME AllTests COMMAND test_suite)
