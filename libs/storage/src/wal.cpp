#include "../include/wal.h"
#include "../include/serialization.h"
#include <exception>
#include <future>
#include <iostream>
#include <optional>
#include <thread>

void write_entry(std::ofstream &f, const WalEntry &entry);
void read_entry(std::ifstream &f, WalEntry &entry);

WalManager::WalManager() {
  std::cout << "[DEBUG] Opening WAL file for writing\n";
  wstream_ = std::ofstream("testwal.bin", std::ios::binary | std::ios::app);
  if (!wstream_)
    throw std::runtime_error("Failed to open file");

  std::cout << "[DEBUG] Starting writer thread\n";
  writer_thread_ = std::thread([this]() { handle_writes(); });
}

WalManager::~WalManager() {
  std::promise<WalResult> promise;
  std::future<WalResult> future = promise.get_future();

  // Close the channel
  printf("Closing channel\n");
  ch_.close();

  // Join the worker thread
  printf("Joining worker thread\n");
  writer_thread_.join();
}

std::future<WalResult> WalManager::set(const std::string &key,
                                       const std::vector<std::uint8_t> &data) {
  std::promise<WalResult> promise;
  std::future<WalResult> future = promise.get_future();
  WalCommand cmd(WalCommandType::Set, key, data, std::move(promise));
  ch_.send(std::move(cmd));
  return future;
}

std::future<WalResult> WalManager::remove(const std::string &key) {
  std::promise<WalResult> promise;
  std::future<WalResult> future = promise.get_future();
  WalCommand cmd(WalCommandType::Remove, key, std::move(promise));
  ch_.send(std::move(cmd));
  return future;
}

void WalManager::stream_entries(
    std::function<void(const WalEntry &)> callback) {
  std::ifstream f("testwal.bin", std::ios::binary);
  if (!f)
    throw new std::runtime_error("Failed to open WAL file");

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
      handle_set(cmd);
      break;
    case WalCommandType::Remove:
      handle_remove(cmd);
      break;
    }
  } catch (...) {
    cmd.promise().set_exception(std::current_exception());
  }
}

/// @brief Writes a SET entry to the WAL file
void WalManager::handle_set(WalCommand &cmd) {
  std::cout << "[DEBUG] handle_set invoked\n";
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
  std::cout << "[DEBUG] write_entry finished" << std::endl;
}

/// @brief Reads a WalEntry from the given file stream
void read_entry(std::ifstream &f, WalEntry &entry) {
  read_bytes(f, entry.type);
  read_bytes(f, entry.key);
  read_bytes(f, entry.data);
}
