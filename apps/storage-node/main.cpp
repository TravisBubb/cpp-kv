#include "../../libs/common/include/logging.h"
#include "grpcpp/server_builder.h"
#include "in_memory_storage_service.h"
#include <csignal>

std::unique_ptr<grpc::Server> server;
std::atomic<bool> shutdown_requested(false);

void HandleShutdownSignal(int signum) {
  LOG_INFO("Signal {} received. Shutting down server...", signum);
  shutdown_requested = true;
  if (server) {
    server->Shutdown();
  }
}

void RunServer() {
  std::string server_addr("0.0.0.0:50051");
  InMemoryStorageServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  server = builder.BuildAndStart();
  LOG_INFO("Server listening on {}", server_addr);
  server->Wait();
  LOG_INFO("Server stopped");
}

int main() {
  logging::init();

  // Setup signal handler
  std::signal(SIGINT, HandleShutdownSignal);
  std::signal(SIGTERM, HandleShutdownSignal);

  std::thread server_thread(RunServer);

  while (!shutdown_requested) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // Wait for server thread to finish after shutdown
  if (server_thread.joinable()) {
    server_thread.join();
  }

  LOG_INFO("Server shut down complete");

  return 0;
}
