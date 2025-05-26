#include "../include/in_memory_engine.h"
#include <mutex>

StorageResult InMemoryStorageEngine::hydrate() {
  std::unique_lock lock(store_mutex_);

  wal_.stream_entries([this](const WalEntry &entry) {
    switch (entry.type) {
    case WalCommandType::Set:
      store_[entry.key] = std::move(entry.data);
      break;
    case WalCommandType::Remove:
      store_.erase(entry.key);
      break;
    }
  });

  return StorageStatus::OK;
}

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
  std::unique_lock lock(store_mutex_); // exclusive write lock
  WalResult wal_result = wal_.set(key, data).get();
  if (wal_result != WalResult::OK) {
    return StorageStatus::WalError;
  }
  store_[key] = data;
  return StorageStatus::OK;
}

StorageStatus InMemoryStorageEngine::set(const std::string &key,
                                         std::vector<std::uint8_t> &&data) {
  std::unique_lock lock(store_mutex_); // exclusive write lock
  WalResult wal_result = wal_.set(key, data).get();
  if (wal_result != WalResult::OK) {
    return StorageStatus::WalError;
  }
  store_[key] = std::move(data);
  return StorageStatus::OK;
}

StorageStatus InMemoryStorageEngine::remove(const std::string &key) {
  std::unique_lock lock(store_mutex_); // exclusive write lock
  WalResult wal_result = wal_.remove(key).get();
  if (wal_result != WalResult::OK) {
    return StorageStatus::WalError;
  }
  size_t erased = store_.erase(key);
  return erased > 0 ? StorageStatus::OK : StorageStatus::NotFound;
}
