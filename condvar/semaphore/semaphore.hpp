#pragma once

#include <twist/stdlike/mutex.hpp>
#include <twist/stdlike/condition_variable.hpp>

// std::lock_guard, std::unique_lock
#include <mutex>
#include <cstdint>

namespace solutions {

// A Counting semaphore

// Semaphores are often used to restrict the number of threads
// than can access some (physical or logical) resource

class Semaphore {
 public:
  // Creates a Semaphore with the given number of permits
  explicit Semaphore(size_t permissions) : permissions_(permissions) {
  }

  // Acquires a permit from this semaphore,
  // blocking until one is available
  void Acquire() {
    std::unique_lock lock(mutex_);

    while (permissions_ == 0) {
      condvar_.wait(lock);
    }

    --permissions_;
  }

  // Releases a permit, returning it to the semaphore
  void Release() {
    std::lock_guard lock(mutex_);
    ++permissions_;
    condvar_.notify_one();
  }

 private:
  size_t permissions_;  // guarded by mutex_;
  twist::stdlike::condition_variable condvar_;
  twist::stdlike::mutex mutex_;

  // Permits
};

}  // namespace solutions
