#include "../../libs/common/include/logging.h"
#include "grpcpp/server_builder.h"
#include "in_memory_storage_service.h"

void RunServer() {
  std::string server_addr("0.0.0.0:50051");
  InMemoryStorageServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  LOG_INFO("Server listening on {}", server_addr);

  server->Wait();
}

int main() {
  logging::init();
  RunServer();
  return 0;
}
