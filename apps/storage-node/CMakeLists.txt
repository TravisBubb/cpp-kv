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

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED libsystemd)
include_directories(${SYSTEMD_INCLUDE_DIRS})
link_directories(${SYSTEMD_LIBRARY_DIRS})

target_link_libraries(storage_node
    PRIVATE 
        storage 
        common
        grpc++
        protobuf::libprotobuf
        ${SYSTEMD_LIBRARIES}
)

target_compile_options(storage_node PRIVATE -Wall -Wextra -Wpedantic -g)

