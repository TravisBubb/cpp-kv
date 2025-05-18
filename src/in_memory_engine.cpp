#include "../include/in_memory_engine.h"

StorageResult InMemoryStorageEngine::get(const std::string &key) {
  auto it = store_.find(key);
  if (it != store_.end()) {
    return StorageResult(it->second, StorageStatus::OK);
  }

  return StorageResult(StorageStatus::NotFound);
}

StorageStatus
InMemoryStorageEngine::set(const std::string &key,
                           const std::vector<std::uint8_t> &data) {
  store_[key] = data;
  return StorageStatus::OK;
}

StorageStatus InMemoryStorageEngine::set(const std::string &key,
                                         std::vector<std::uint8_t> &&data) {
  store_[key] = std::move(data);
  return StorageStatus::OK;
}

StorageStatus InMemoryStorageEngine::remove(const std::string &key) {
  size_t erased = store_.erase(key);
  return erased > 0 ? StorageStatus::OK : StorageStatus::NotFound;
}
