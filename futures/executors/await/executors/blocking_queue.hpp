#pragma once

#include <twist/stdlike/mutex.hpp>
#include <twist/stdlike/atomic.hpp>
#include <twist/stdlike/condition_variable.hpp>

#include <mutex>
#include <queue>
#include <optional>
#include <iostream>

namespace await::executors {

// Unbounded blocking multi-producers/multi-consumers queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  bool Put(T element) {
    if (closed_.load() != 0u) {
      return false;
    }

    std::lock_guard lock(mutex_);
    queue_.push(std::move(element));
    not_empty_.notify_one();

    return true;
  }

  std::optional<T> Take() {
    std::unique_lock lock(mutex_);

    while (queue_.empty() && ((closed_.load()) == 0u)) {
      not_empty_.wait(lock);
    }

    if (closed_.load() && queue_.empty()) {
      return std::nullopt;
    }

    auto task = std::optional{std::move(queue_.front())};
    queue_.pop();

    return task;
  }

  void Close() {
    CloseImpl(/*clear=*/false);
  }

  void Cancel() {
    CloseImpl(/*clear=*/true);
  }

 private:
  void CloseImpl(bool clear) {
    std::lock_guard lock(mutex_);

    closed_.store(1);

    if (clear) {
      queue_ = {};
    }

    not_empty_.notify_all();
  }

 private:
  twist::stdlike::atomic<uint32_t> closed_;
  std::queue<T> queue_;  // guarded by mutex_
  twist::stdlike::condition_variable not_empty_;
  twist::stdlike::mutex mutex_;
};

}  // namespace await::executors
