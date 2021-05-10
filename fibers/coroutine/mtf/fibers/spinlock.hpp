#include <twist/stdlike/atomic.hpp>

namespace mtf::spinlock {
class TATASSpinlock {
 public:
  TATASSpinlock() = default;
  TATASSpinlock(const TATASSpinlock& other) = delete;
  TATASSpinlock& operator=(const TATASSpinlock& other) = delete;

  void lock() {  // NOLINT
    do {
      while (busy_.load() != 0u) {
      }
    } while (busy_.exchange(1) != 0u);
  }

  void unlock() {  // NOLINT
    busy_.store(0);
  }

 private:
  twist::stdlike::atomic<uint32_t> busy_;
};
};  // namespace mtf::spinlock
