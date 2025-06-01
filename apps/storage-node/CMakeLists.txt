add_executable(storage_node
    main.cpp
    in_memory_storage_service.cpp
    ${CMAKE_BINARY_DIR}/proto/storage.pb.cc
    ${CMAKE_BINARY_DIR}/proto/storage.grpc.pb.cc
)

target_include_directories(storage_node
    PRIVATE 
        ${CMAKE_BINARY_DIR}/proto
        ${grpc_SOURCE_DIR}/third_party/protobuf/src
)

target_link_libraries(storage_node
    PRIVATE 
        storage 
        common
        grpc++
        protobuf::libprotobuf
)

target_compile_options(storage_node PRIVATE -Wall -Wextra -Wpedantic -g)

