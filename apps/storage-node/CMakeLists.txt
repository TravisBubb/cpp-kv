add_executable(storage_node
    main.cpp
)

target_link_libraries(storage_node
    PRIVATE storage common
)
target_compile_options(storage_node PRIVATE -Wall -Wextra -Wpedantic -g)

