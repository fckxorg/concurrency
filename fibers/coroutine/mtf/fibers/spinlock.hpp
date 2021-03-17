#include <atomic>

namespace mtf::spinlock {
class TATASSpinlock {
 public:
  TATASSpinlock() = default;
  TATASSpinlock(const TATASSpinlock& other) = delete;
  TATASSpinlock& operator=(const TATASSpinlock& other) = delete;

  void lock();
  void unlock();

 private:
  std::atomic<bool> busy_;
};
};  // namespace mtf::spinlock
