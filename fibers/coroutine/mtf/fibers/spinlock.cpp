#include "spinlock.hpp"

namespace mtf::spinlock {
void TATASSpinlock::lock() {
  do {
    while (busy_.load()) {
    }
  } while (busy_.exchange(true));
}

void TATASSpinlock::unlock() {
  busy_.store(false);
}
};  // namespace mtf::spinlock
