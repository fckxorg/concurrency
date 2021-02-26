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

    mutex.unlock();
    state_.wait(old);
    mutex.lock();
  }

  void NotifyOne() {
    state_.fetch_add(1);
    state_.notify_one();
  }

  void NotifyAll() {
    state_.fetch_add(1);
    state_.notify_all();
  }

 private:
  twist::stdlike::atomic<uint32_t> state_;
};

}  // namespace solutions
