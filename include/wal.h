#ifndef WAL_H
#define WAL_H

#include "channel.h"
#include <cstdint>
#include <future>
#include <optional>
#include <string>
#include <thread>
#include <vector>

/// @brief Represents a type of command that can be sent from the WAL producers
/// to the consumer
enum class WalCommandType { Set, Remove, Flush, Shutdown };

/// @brief Represents the result of an interaction with the WAL
enum class WalResult { OK, IOError };

/// @brief Represents a command to send from the WAl producers to the consumer
class WalCommand {
public:
  WalCommand(WalCommandType type, std::promise<WalResult> &&promise)
      : type_(type), promise_(std::move(promise)) {}

  WalCommand(WalCommandType type, std::string key,
             std::promise<WalResult> &&promise)
      : type_(type), key_(std::move(key)), promise_(std::move(promise)) {}

  WalCommand(WalCommandType type, std::string key,
             std::vector<std::uint8_t> data, std::promise<WalResult> &&promise)
      : type_(type), key_(std::move(key)), data_(std::move(data)),
        promise_(std::move(promise)) {}

  std::promise<WalResult> &promise() { return promise_; }
  WalCommandType get_type() const { return type_; }
  const std::string &key() const { return key_; }
  const std::vector<std::uint8_t> &data() const { return data_; }

private:
  WalCommandType type_;
  std::string key_;
  std::vector<std::uint8_t> data_;
  std::promise<WalResult> promise_;
};

/// @brief Handles Write-Ahead Log (WAL) write and read operations
class WalManager {
public:
  /// @brief Constructs a new WalManager
  ///
  /// Starts a new background thread to handle all writes to the WAL
  WalManager();

  /// @brief Destroys the WalManager
  ~WalManager();

  /// @brief Writes a SET operation to the WAL
  ///
  /// @returns OK if successful, otherwise an error type
  std::future<WalResult> set(const std::string &key,
                             const std::vector<std::uint8_t> &data);

  /// @brief Writes a REMOVE operation to the WAL
  ///
  /// @returns OK if successful, otherwise an error type
  std::future<WalResult> remove(const std::string &key);

private:
  void handle_writes();

  std::thread writer_thread_;
  Channel<WalCommand> ch_;
};

#endif
