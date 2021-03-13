#pragma once

#include <bits/stdint-intn.h>
#include <twist/stdlike/atomic.hpp>
#include <twist/util/spin_wait.hpp>

#include <cstdlib>

namespace solutions {

using twist::util::SpinWait;

class Mutex {
 public:
  void Lock() {
    while (locked_.exchange(1) != 0) {
      n_threads_.fetch_add(1);
      locked_.wait(1);
      n_threads_.fetch_sub(1);
    }
  }

  void Unlock() {
    locked_.exchange(0);

    if (n_threads_.load()) {
      locked_.notify_one();
    }
  }

 private:
  twist::stdlike::atomic<unsigned int> locked_;
  twist::stdlike::atomic<unsigned int> n_threads_{0};
};

}  // namespace solutions
