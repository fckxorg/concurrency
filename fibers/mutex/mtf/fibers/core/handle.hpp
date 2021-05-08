#pragma once
#include <twist/stdlike/mutex.hpp>
#include <mutex>
#include <wheels/support/intrusive_list.hpp>
#include <mtf/support/spinlock.hpp>

namespace mtf::fibers {

class Awaiter;

// Lightweight non-owning handle to the fiber object
class FiberHandle : public wheels::IntrusiveListNode<FiberHandle> {
 public:
  explicit FiberHandle(void* fiber) : fiber_(fiber) {
  }

  FiberHandle() : FiberHandle(nullptr) {
  }

  static FiberHandle Invalid() {
    return FiberHandle(nullptr);
  }

  bool IsValid() const {
    return fiber_ != nullptr;
  }

  void Suspend(Awaiter* awaiter);

  void Resume();

 private:
  void* fiber_;
};

class Awaiter {
 private:
  wheels::IntrusiveList<FiberHandle>* queue_;
  mtf::support::SpinLock& lock_;
  FiberHandle handle_;

 public:
  Awaiter(wheels::IntrusiveList<FiberHandle>* queue,
          mtf::support::SpinLock& lock)
      : queue_(queue), lock_(lock) {
  }

  void AwaitSuspend(FiberHandle handle) {
    handle_ = handle;
    queue_->PushBack(&handle_);
    lock_.unlock();
  }
  void AwaitResume() {
  }
};

}  // namespace mtf::fibers
