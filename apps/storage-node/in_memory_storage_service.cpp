#include "in_memory_storage_service.h"
#include "engine.h"
#include "storage.pb.h"
#include <cstdint>

grpc::Status InMemoryStorageServiceImpl::Get(grpc::ServerContext *,
                                             const storage::GetRequest *request,
                                             storage::GetResponse *response) {
  if (!request || !response) {
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                        "Null request or response");
  }

  const std::string &key = request->key();
  StorageResult result = engine_.get(key);
  if (result.get_status() == StorageStatus::OK) {
    if (!result.get_data().has_value()) {
      return grpc::Status(grpc::StatusCode::NOT_FOUND,
                          "Requested key was not found");
    }

    const std::vector<uint8_t> &bytes = result.get_data().value();
    storage::Value *value = response->mutable_value();
    value->set_data(reinterpret_cast<const char *>(bytes.data()), bytes.size());
    return grpc::Status::OK;
  }

  return grpc::Status(grpc::StatusCode::INTERNAL,
                      "An unexpected error occurred attempting to lookup key");
}

grpc::Status InMemoryStorageServiceImpl::Set(grpc::ServerContext *,
                                             const storage::SetRequest *request,
                                             storage::SetResponse *response) {
  if (!request || !response) {
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                        "Null request or response");
  }

  const std::string &key = request->key();
  const storage::Value &value = request->value();
  const std::string &data = value.data();
  const std::vector<uint8_t> bytes(data.begin(), data.end());
  StorageResult result = engine_.set(key, bytes);

  if (result.get_status() == StorageStatus::OK) {
    return grpc::Status::OK;
  }

  return grpc::Status(
      grpc::StatusCode::INTERNAL,
      "An unexpected error occurred attempting to store key-value pair");
}
