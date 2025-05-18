#include "../include/in_memory_engine.h"
#include <mutex>

StorageResult InMemoryStorageEngine::get(const std::string &key) const {
  std::shared_lock lock(store_mutex_); // shared read lock
  auto it = store_.find(key);
  if (it != store_.end()) {
    return StorageResult(it->second, StorageStatus::OK);
  }

  return StorageResult(StorageStatus::NotFound);
}

StorageStatus
InMemoryStorageEngine::set(const std::string &key,
                           const std::vector<std::uint8_t> &data) {
  std::unique_lock lock(store_mutex_); // exclusive write lovk
  store_[key] = data;
  return StorageStatus::OK;
}

StorageStatus InMemoryStorageEngine::set(const std::string &key,
                                         std::vector<std::uint8_t> &&data) {
  std::unique_lock lock(store_mutex_); // exclusive write lock
  store_[key] = std::move(data);
  return StorageStatus::OK;
}

StorageStatus InMemoryStorageEngine::remove(const std::string &key) {
  std::unique_lock lock(store_mutex_); // exclusive write lock
  size_t erased = store_.erase(key);
  return erased > 0 ? StorageStatus::OK : StorageStatus::NotFound;
}
