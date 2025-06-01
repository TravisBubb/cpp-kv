#include "grpcpp/server_builder.h"
#include "in_memory_engine.h"
#include "in_memory_storage_service.h"
#include <iostream>

void RunServer() {
  std::string server_addr("0.0.0.0:50051");
  InMemoryStorageServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "[DEBUG] Server listening on " << server_addr << std::endl;

  server->Wait();
}

int main() {
  {
    auto engine = InMemoryStorageEngine();
    std::string v1 = "{ \"name\": \"travis\" }";
    std::printf("Setting k1:v1\n");
    engine.set("k1", std::vector<uint8_t>(v1.begin(), v1.end()));
    StorageResult res = engine.get("k1");
    if (res.get_status() == StorageStatus::OK) {
      if (!res.get_data().has_value()) {
        std::printf("No value found for k1\n");
        return 1;
      }
      std::vector<std::uint8_t> bytes = res.get_data().value();
      std::string str = std::string(bytes.begin(), bytes.end());
      std::printf("Retrieved k1:%s\n", str.c_str());
    } else {
      std::printf("Received error status\n");
    }
  }

  auto hydrated_engine = InMemoryStorageEngine();
  hydrated_engine.hydrate();

  StorageResult res = hydrated_engine.get("k1");
  if (res.get_status() == StorageStatus::OK) {
    if (!res.get_data().has_value()) {
      std::printf("No value found for k1\n");
      return 1;
    }
    std::vector<std::uint8_t> bytes = res.get_data().value();
    std::string str = std::string(bytes.begin(), bytes.end());
    std::printf("Retrieved k1:%s\n", str.c_str());
  } else {
    std::printf("Received error status\n");
  }

  RunServer();

  return 0;
}
