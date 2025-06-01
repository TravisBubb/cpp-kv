#ifndef CHANNEL_H
#define CHANNEL_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <utility>

/// @class Channel
/// @brief A channel implementation for inter-thread communication
///
/// @note All public methods are thread-safe. `send()` and `close()` may be
/// called from multiple producers; `receive()` may be called from multiple
/// consumers.
template <typename T> class Channel {
public:
  Channel() = default;
  ~Channel() = default;

  // Delete the copy and move constructors since we are using a mutex and
  // condition variable.
  Channel(const Channel &) = delete;
  Channel &operator=(const Channel) = delete;
  Channel(Channel &&) = delete;
  Channel &operator=(Channel &&) = delete;

  /// @brief Push a value to the channel
  ///
  /// Uses a mutex to synchronize access to the queue. Notifies waiting consumer
  /// threads with a condition variable.
  ///
  /// @param value The value to send to the channel.
  template <typename U> void send(U &&value) {
    {

      std::lock_guard<std::mutex> lock(mtx_);
      queue_.emplace_back(std::forward<U>(value));
    }
    cv_.notify_one();
  }

  /// @brief Receive a value from the channel, blocking until either the channel
  /// is closed or a value is received.
  ///
  /// Waits on condition variable to be notified that an item is ready to be
  /// received. When a value is ready to be received, pop it from the queue and
  /// return the value. If the channel is closed, all remaining values in the
  /// queue will be received before finally receiving nullopt.
  ///
  /// @return The value received from the queue, otherwise nullopt.
  std::optional<T> receive() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&] { return closed_ || !queue_.empty(); });

    if (queue_.empty())
      return std::nullopt;

    T v = std::move(queue_.front());
    queue_.pop_front();
    return v;
  }

  /// @brief Signal that no more values will be sent through the channel
  ///
  /// Signals to all consumer threads waiting on the condition variable that the
  /// channel has been closed. Any remaining values in the queue will be
  /// available to receive after closing.
  void close() {
    {
      std::lock_guard lock(mtx_);
      closed_ = true;
    }
    cv_.notify_all();
  }

private:
  bool closed_ = false;
  std::deque<T> queue_;
  std::mutex mtx_;
  std::condition_variable cv_;
};

#endif
