#ifndef IN_MEMORY_ENGINE
#define IN_MEMORY_ENGINE

#include "engine.h"
#include <unordered_map>

class InMemoryStorageEngine : public StorageEngine {
public:
  InMemoryStorageEngine() = default;
  ~InMemoryStorageEngine() override = default;

  StorageResult get(const std::string &key) override;
  StorageStatus set(const std::string &key,
                    const std::vector<std::uint8_t> &data) override;
  StorageStatus set(const std::string &key,
                    std::vector<std::uint8_t> &&data) override;
  StorageStatus remove(const std::string &key) override;

private:
  std::unordered_map<std::string, std::vector<std::uint8_t>> store_;
};

#endif
