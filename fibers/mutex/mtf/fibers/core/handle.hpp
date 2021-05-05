#pragma once
#include <twist/stdlike/mutex.hpp>
#include <mutex>
#include <wheels/support/intrusive_list.hpp>

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
  twist::stdlike::mutex& mutex_;
  FiberHandle handle_;

 public:
  Awaiter(wheels::IntrusiveList<FiberHandle>* queue,
          twist::stdlike::mutex& mutex)
      : queue_(queue), mutex_(mutex) {
  }

  void AwaitSuspend(FiberHandle handle) {
    handle_ = handle;
    queue_->PushBack(&handle_);
    mutex_.unlock();
  }
  void AwaitResume() {
    delete this;
  }
};

}  // namespace mtf::fibers
