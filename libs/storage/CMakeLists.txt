add_library(storage
    src/in_memory_engine.cpp
    src/serialization.cpp
    src/wal.cpp
)

target_include_directories(storage
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(storage PRIVATE -Wall -Wextra -Wpedantic -g)

target_link_libraries(storage PUBLIC common)

# Unit tests
add_executable(storage_tests
    tests/test_wal.cpp
)

target_link_libraries(storage_tests 
    PRIVATE 
        storage 
        common
        GTest::gtest_main
)

add_test(NAME StorageTests COMMAND storage_tests)

