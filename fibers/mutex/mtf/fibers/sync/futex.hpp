#pragma once

#include <mtf/fibers/core/handle.hpp>
#include <mtf/fibers/core/suspend.hpp>

#include <mutex>
#include <twist/stdlike/atomic.hpp>

#include <wheels/support/intrusive_list.hpp>

namespace mtf::fibers {

template <typename T>
class FutexLike : public twist::stdlike::atomic<T>,
                  public wheels::IntrusiveList<FiberHandle> {
 private:
  mtf::support::SpinLock mutex_;

 public:
  explicit FutexLike(T initial_value)
      : twist::stdlike::atomic<T>(initial_value) {
  }

  ~FutexLike() {
    // Not implemented
    // Check that wait queue is empty
  }

  // Park current fiber if value of atomic is equal to `old`
  void ParkIfEqual(T old) {
    mutex_.lock();
    if (this->load() != old) {
      mutex_.unlock();
      return;
    }

    Awaiter* awaiter = new Awaiter{this, mutex_};
    Suspend(awaiter);
  }

  void WakeOne() {
    std::lock_guard lock(mutex_);
    if (this->Size()) {
      this->PopFront()->Resume();
    }
  }

  void WakeAll() {
    std::lock_guard lock(mutex_);
    for (auto size = this->Size(); size >= 0; --size) {
      this->PopFront()->Resume();
    }
  }
};

}  // namespace mtf::fibers
