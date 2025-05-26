#ifndef WAL_H
#define WAL_H

#include "channel.h"
#include <cstdint>
#include <fstream>
#include <future>
#include <optional>
#include <string>
#include <thread>
#include <vector>

/// @brief Represents a type of command that can be sent from the WAL producers
/// to the consumer
enum class WalCommandType : uint8_t { Set, Remove };

/// @brief Represents the result of an interaction with the WAL
enum class WalResult { OK, IOError };

/// @brief Represents a single entry in the WAL
struct WalEntry {
  WalCommandType type;
  std::string key;
  std::vector<uint8_t> data;
};

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
  /// @brief Infinitely process all incoming commands
  void handle_writes();

  /// @brief Retrieve a WalCommand from the channel and route to the proper
  /// handler
  void handle_cmd(WalCommand &cmd);

  /// @brief Handle the set command
  void handle_set(WalCommand &cmd);

  /// @brief Handle the remove command
  void handle_remove(WalCommand &cmd);

  std::thread writer_thread_;
  Channel<WalCommand> ch_;
  std::ofstream wstream_;
};

#endif
