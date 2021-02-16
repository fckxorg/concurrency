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
      locked_.wait(1);
    }
  }

  void Unlock() {
    locked_.exchange(0);
    locked_.notify_one();
  }

 private:
  twist::stdlike::atomic<unsigned int> locked_;
};

}  // namespace solutions
