#pragma once

#include <twist/stdlike/atomic.hpp>
#include <twist/util/spin_wait.hpp>

namespace mtf::support {

class SpinLock {
 public:
  void Lock() {
    twist::util::SpinWait spin_wait;
    while (locked_.exchange(1) != 0u) {
      spin_wait();
    }
  }

  void Unlock() {
    locked_.store(0);
  }

  // NOLINTNEXTLINE
  void lock() {
    Lock();
  }

  // NOLINTNEXTLINE
  void unlock() {
    Unlock();
  }

 private:
  twist::stdlike::atomic<uint32_t> locked_{0};
};

}  // namespace mtf::support
