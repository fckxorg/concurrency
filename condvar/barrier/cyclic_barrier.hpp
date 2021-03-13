#pragma once

#include <twist/stdlike/mutex.hpp>
#include <twist/stdlike/condition_variable.hpp>

// std::lock_guard, std::unique_lock
#include <mutex>
#include <cstdint>

namespace solutions {

// CyclicBarrier allows a set of threads to all wait for each other
// to reach a common barrier point

// The barrier is called cyclic because
// it can be re-used after the waiting threads are released.

class CyclicBarrier {
 public:
  explicit CyclicBarrier(size_t n_participants)
      : n_participants_(n_participants), arrived_(0), state_(false) {
  }

  // Blocks until all participants have invoked Arrive()
  void Arrive() {
    std::unique_lock lock(mutex_);

    bool current_state = state_;

    ++arrived_;
    if (arrived_ == n_participants_) {
      arrived_ = 0;
      state_ = !state_;
      condvar_.notify_all();
    }

    while (current_state == state_) {
      condvar_.wait(lock);
    }
  }

 private:
  const size_t n_participants_;
  size_t arrived_;  // guarded by mutex_
  bool state_;
  twist::stdlike::condition_variable condvar_;
  twist::stdlike::mutex mutex_;
};

}  // namespace solutions
