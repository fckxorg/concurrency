#pragma once

#include <mtf/fibers/sync/futex.hpp>
// Silly bump
namespace mtf::fibers {
class Mutex {
 public:
  void Lock() {
    while (locked_.exchange(1) != 0) {
      n_threads_.fetch_add(1);
      locked_.ParkIfEqual(1);
      n_threads_.fetch_sub(1);
    }
  }

  void Unlock() {
    locked_.exchange(0);

    if (n_threads_.load() != 0u) {
      locked_.WakeOne();
    }
  }

  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  FutexLike<unsigned int> locked_{0};
  twist::stdlike::atomic<unsigned int> n_threads_{0};
};

}  // namespace mtf::fibers
