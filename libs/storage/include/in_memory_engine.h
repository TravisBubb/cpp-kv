#ifndef IN_MEMORY_ENGINE
#define IN_MEMORY_ENGINE

#include "engine.h"
#include "wal.h"
#include <shared_mutex>
#include <unordered_map>

class InMemoryStorageEngine : public StorageEngine {
public:
  InMemoryStorageEngine() = default;
  ~InMemoryStorageEngine() override = default;

  StorageResult hydrate();
  StorageResult get(const std::string &key) const override;
  StorageStatus set(const std::string &key,
                    const std::vector<std::uint8_t> &data) override;
  StorageStatus set(const std::string &key,
                    std::vector<std::uint8_t> &&data) override;
  StorageStatus remove(const std::string &key) override;

private:
  std::unordered_map<std::string, std::vector<std::uint8_t>> store_;
  mutable std::shared_mutex store_mutex_;
  WalManager wal_;
};

#endif
