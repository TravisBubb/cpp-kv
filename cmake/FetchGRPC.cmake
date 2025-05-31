include(FetchContent)

# Let gRPC build and manage its own dependencies
set(FETCHCONTENT_QUIET FALSE)
set(gRPC_BUILD_TESTS OFF)
set(gRPC_INSTALL OFF)
set(protobuf_INSTALL OFF)
set(utf8_range_INSTALL OFF)
set(absl_ENABLE_INSTALL OFF)
set(utf8_range_SKIP_INSTALL ON)

# This ensures 'install()' commands in subprojects do nothing
macro(install)
  # no-op to suppress install commands from dependencies
endmacro()

set(gRPC_ABSL_PROVIDER "module")
set(gRPC_RE2_PROVIDER "module")
set(gRPC_CARES_PROVIDER "module")
set(gRPC_SSL_PROVIDER "module")
set(gRPC_ZLIB_PROVIDER "module")
set(gRPC_BENCHMARK_PROVIDER "module")
set(gRPC_PROTOBUF_PROVIDER "module")

FetchContent_Declare(
  grpc
  GIT_REPOSITORY https://github.com/grpc/grpc
  GIT_TAG        v1.72.0
)
FetchContent_MakeAvailable(grpc)
