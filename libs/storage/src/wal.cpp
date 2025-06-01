#include "../include/wal.h"
#include "../../common/include/logging.h"
#include "../include/serialization.h"
#include <exception>
#include <future>
#include <optional>
#include <thread>

void write_entry(std::ofstream &f, const WalEntry &entry);
void read_entry(std::ifstream &f, WalEntry &entry);

WalManager::WalManager() {
  LOG_DEBUG("Opening WAL file for writing");
  wstream_ = std::ofstream("testwal.bin", std::ios::binary | std::ios::app);
  if (!wstream_) {
    LOG_ERROR("Failed to open WAL file");
    throw std::runtime_error("Failed to open file");
  }

  LOG_DEBUG("Starting WAL writer thread");
  writer_thread_ = std::thread([this]() { handle_writes(); });
}

WalManager::~WalManager() {
  std::promise<WalResult> promise;
  std::future<WalResult> future = promise.get_future();

  // Close the channel
  LOG_DEBUG("Closing WAL writer channel");
  ch_.close();

  // Join the worker thread
  LOG_DEBUG("Joining WAL worker thread");
  writer_thread_.join();
}

std::future<WalResult> WalManager::set(const std::string &key,
                                       const std::vector<std::uint8_t> &data) {
  std::promise<WalResult> promise;
  std::future<WalResult> future = promise.get_future();
  WalCommand cmd(WalCommandType::Set, key, data, std::move(promise));
  LOG_DEBUG("Sending SET command to WAL channel");
  ch_.send(std::move(cmd));
  return future;
}

std::future<WalResult> WalManager::remove(const std::string &key) {
  std::promise<WalResult> promise;
  std::future<WalResult> future = promise.get_future();
  WalCommand cmd(WalCommandType::Remove, key, std::move(promise));
  LOG_DEBUG("Sending GET command to WAL channel");
  ch_.send(std::move(cmd));
  ch_.send(std::move(cmd));
  return future;
}

void WalManager::stream_entries(
    std::function<void(const WalEntry &)> callback) {
  std::ifstream f("testwal.bin", std::ios::binary);
  if (!f) {
    LOG_ERROR("Failed to open WAL file for hydration");
    throw new std::runtime_error("Failed to open WAL file");
  }

  LOG_DEBUG("Streaming WAL entries to hydrate the store");

  WalEntry entry;
  while (f.peek() != EOF) {
    read_entry(f, entry);
    callback(entry);
  }
}

void WalManager::handle_writes() {
  while (true) {
    std::optional<WalCommand> received = ch_.receive();
    if (!received.has_value())
      break;

    WalCommand &cmd = received.value();
    handle_cmd(cmd);
  }
}

void WalManager::handle_cmd(WalCommand &cmd) {
  try {
    switch (cmd.get_type()) {
    case WalCommandType::Set:
      LOG_DEBUG("Handling WAL SET command");
      handle_set(cmd);
      break;
    case WalCommandType::Remove:
      LOG_DEBUG("Handling WAL REMOVE command");
      handle_remove(cmd);
      break;
    }
  } catch (...) {
    LOG_ERROR("An exception occurred attempting to handle WAL command");
    cmd.promise().set_exception(std::current_exception());
  }
}

/// @brief Writes a SET entry to the WAL file
void WalManager::handle_set(WalCommand &cmd) {
  WalEntry entry = {
      .type = WalCommandType::Set, .key = cmd.key(), .data = cmd.data()};
  write_entry(wstream_, entry);
  cmd.promise().set_value(WalResult::OK);
}

/// @brief Writes a REMOVE entry to the WAl file
void WalManager::handle_remove(WalCommand &cmd) {
  WalEntry entry = {
      .type = WalCommandType::Remove, .key = cmd.key(), .data = {}};
  write_entry(wstream_, entry);
  cmd.promise().set_value(WalResult::OK);
}

/// @brief Writes a WalEntry to the given file stream
void write_entry(std::ofstream &f, const WalEntry &entry) {
  write_bytes(f, entry.type);
  write_bytes(f, entry.key);
  write_bytes(f, entry.data);
  f.flush();
}

/// @brief Reads a WalEntry from the given file stream
void read_entry(std::ifstream &f, WalEntry &entry) {
  read_bytes(f, entry.type);
  read_bytes(f, entry.key);
  read_bytes(f, entry.data);
}
