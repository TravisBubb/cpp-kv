file(GLOB COMMON_SRC CONFIGURE_DEPENDS "lib/common/*.cpp")

if(COMMON_SRC)
    add_library(common STATIC ${COMMON_SRC})
    target_include_directories(common PUBLIC include/)
    target_compile_options(common PRIVATE -Wall -Wextra)
else()
    add_library(common INTERFACE)
    target_include_directories(common INTERFACE include/)
    target_compile_options(common INTERFACE -Wall -Wextra)
endif()

# Unit tests
add_executable(common_tests
    tests/test_channel.cpp
)
target_link_libraries(common_tests PRIVATE common GTest::gtest_main)
add_test(NAME CommonTests COMMAND common_tests)

