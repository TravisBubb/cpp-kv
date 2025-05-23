#include "../include/wal.h"
#include <exception>
#include <future>
#include <optional>
#include <thread>

WalManager::WalManager() {
  writer_thread_ = std::thread([this]() { handle_writes(); });
}

WalManager::~WalManager() {
  std::promise<WalResult> promise;
  std::future<WalResult> future = promise.get_future();

  // Send the shutdown command to the worker thread
  printf("Sending shutdown cmd to worker thread\n");
  WalCommand cmd(WalCommandType::Shutdown, std::move(promise));
  ch_.send(std::move(cmd));
  WalResult _ = future.get();

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

void WalManager::handle_writes() {
  while (true) {
    std::optional<WalCommand> received = ch_.receive();
    if (!received.has_value())
      break;

    WalCommand &cmd = received.value();
    try {
      switch (cmd.get_type()) {
      case WalCommandType::Set:
        printf("Set received...\n");
        cmd.promise().set_value(WalResult::OK);
        break;
      case WalCommandType::Remove:
        printf("Remove received...\n");
        cmd.promise().set_value(WalResult::OK);
        break;
      case WalCommandType::Flush:
        printf("Flush received...\n");
        cmd.promise().set_value(WalResult::OK);
        break;
      case WalCommandType::Shutdown:
        printf("Shutdown received...\n");
        cmd.promise().set_value(WalResult::OK);
        break;
      }
    } catch (...) {
      cmd.promise().set_exception(std::current_exception());
    }
  }
}
