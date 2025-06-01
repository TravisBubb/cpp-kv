#include "in_memory_storage_service.h"
#include "../../libs/common/include/logging.h"
#include "engine.h"
#include "storage.pb.h"
#include <cstdint>

grpc::Status InMemoryStorageServiceImpl::Get(grpc::ServerContext *,
                                             const storage::GetRequest *request,
                                             storage::GetResponse *response) {
  LOG_INFO("Received storage.StorageService.Get request");
  if (!request || !response) {
    LOG_WARN("Cannot process request; Either the request or response is null");
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                        "Null request or response");
  }

  const std::string &key = request->key();
  LOG_DEBUG("Attempting to retrieve value for key \"{}\"", key);
  StorageResult result = engine_.get(key);
  if (result.get_status() == StorageStatus::OK) {
    if (!result.get_data().has_value()) {
      LOG_INFO("Key {} was not found", key);
      return grpc::Status(grpc::StatusCode::NOT_FOUND,
                          "Requested key was not found");
    }

    const std::vector<uint8_t> &bytes = result.get_data().value();
    storage::Value *value = response->mutable_value();
    value->set_data(reinterpret_cast<const char *>(bytes.data()), bytes.size());
    LOG_DEBUG("Key \"{}\" was found with value \"{}\"", key, value->data());
    LOG_INFO("Finished processing storage.StorageService.Get request");
    return grpc::Status::OK;
  }

  LOG_ERROR("An unexpected error occurred attempting to process "
            "storage.StorageService.Get request for key \"{}\"",
            key);
  return grpc::Status(grpc::StatusCode::INTERNAL,
                      "An unexpected error occurred attempting to lookup key");
}

grpc::Status InMemoryStorageServiceImpl::Set(grpc::ServerContext *,
                                             const storage::SetRequest *request,
                                             storage::SetResponse *response) {
  LOG_INFO("Received storage.StorageService.Set request");

  if (!request || !response) {
    LOG_WARN("Cannot process request; Either the request or response is null");
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                        "Null request or response");
  }

  const std::string &key = request->key();
  const storage::Value &value = request->value();
  const std::string &data = value.data();
  const std::vector<uint8_t> bytes(data.begin(), data.end());
  LOG_DEBUG("Attempting to set key \"{}\" and value \"{}\"", key, data);
  StorageResult result = engine_.set(key, bytes);

  if (result.get_status() == StorageStatus::OK) {
    LOG_INFO("Finished processing storage.StorageService.Get request");
    return grpc::Status::OK;
  }

  LOG_ERROR(
      "An unexpected error occurred attempting to process "
      "storage.StorageService.Set request for key \"{}\" and value \"{}\"",
      key, data);

  return grpc::Status(
      grpc::StatusCode::INTERNAL,
      "An unexpected error occurred attempting to store key-value pair");
}
