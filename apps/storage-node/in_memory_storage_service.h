#ifndef IN_MEMORY_STORAGE_SERVICE_H
#define IN_MEMORY_STORAGE_SERVICE_H

#include "grpcpp/server.h"
#include "grpcpp/server_context.h"
#include "in_memory_engine.h"
#include "storage.grpc.pb.h"
#include "storage.pb.h"
#include <cstdio>
#include <grpcpp/support/status.h>

class InMemoryStorageServiceImpl final
    : public storage::StorageService::Service {
public:
  InMemoryStorageServiceImpl() = default;
  ~InMemoryStorageServiceImpl() = default;
  grpc::Status Get(grpc::ServerContext *context,
                   const storage::GetRequest *request,
                   storage::GetResponse *response) override;

  grpc::Status Set(grpc::ServerContext *context,
                   const storage::SetRequest *request,
                   storage::SetResponse *response) override;

private:
  InMemoryStorageEngine engine_;
};

#endif
