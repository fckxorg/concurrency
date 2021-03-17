#include <atomic>

namespace mtf::spinlock {
class TATASSpinlock {
 public:
  TATASSpinlock() = default;
  TATASSpinlock(const TATASSpinlock& other) = delete;
  TATASSpinlock& operator=(const TATASSpinlock& other) = delete;

  void lock() {
    do {
      while (busy_.load()) {
      }
    } while (busy_.exchange(true));
  }

  void unlock() {
    busy_.store(false);
  }

 private:
  std::atomic<bool> busy_;
};
};  // namespace mtf::spinlock
