#ifndef ENGINE_H
#define ENGINE_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

enum class StorageStatus {
  OK,
  NotFound,
  WalError,
};

struct StorageResult {
public:
  StorageResult(StorageStatus status) : status_(status) {}

  StorageResult(const std::vector<std::uint8_t> &data, StorageStatus status)
      : data_(data), status_(status) {}

  StorageResult(std::vector<std::uint8_t> &&data, StorageStatus status)
      : data_(std::move(data)), status_(status) {}

  StorageStatus get_status() const { return status_; }

  const std::optional<std::vector<std::uint8_t>> &get_data() const {
    return data_;
  }

private:
  std::optional<std::vector<std::uint8_t>> data_;
  StorageStatus status_;
};

class StorageEngine {
public:
  virtual ~StorageEngine() = default;

  virtual StorageResult get(const std::string &) const = 0;

  virtual StorageStatus set(const std::string &,
                            const std::vector<std::uint8_t> &) = 0;
  virtual StorageStatus set(const std::string &,
                            std::vector<std::uint8_t> &&) = 0;

  virtual StorageStatus remove(const std::string &) = 0;
};

#endif
