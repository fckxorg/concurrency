#pragma once

#include <mtf/fibers/sync/mutex.hpp>
#include <mtf/fibers/sync/futex.hpp>
#include <twist/stdlike/atomic.hpp>

// std::unique_lock
#include <mutex>

namespace mtf::fibers {

class CondVar {
  using Lock = std::unique_lock<Mutex>;

 public:
  void Wait(Lock& lock);

  void NotifyOne();
  void NotifyAll();

 private:
  FutexLike<uint32_t> state_{0};
  twist::stdlike::atomic<uint32_t> n_waiting_{0};
};

}  // namespace mtf::fibers
