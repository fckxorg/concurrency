#pragma once

#include <twist/stdlike/atomic.hpp>
#include <mutex>
#include <cstdint>

namespace solutions {

class ConditionVariable {
 public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    uint32_t old = state_.load();

    n_waiting_.fetch_add(1);
    mutex.unlock();
    state_.wait(old);
    mutex.lock();
    n_waiting_.fetch_sub(1);
  }

  void NotifyOne() {
    if (n_waiting_.load() != 0u) {
      state_.fetch_add(1);
      state_.notify_one();
    }
  }

  void NotifyAll() {
    if (n_waiting_.load() != 0u) {
      state_.fetch_add(1);
      state_.notify_all();
    }
  }

 private:
  twist::stdlike::atomic<uint32_t> state_;
  twist::stdlike::atomic<uint32_t> n_waiting_;
  // TODO counter for wait
};

}  // namespace solutions
