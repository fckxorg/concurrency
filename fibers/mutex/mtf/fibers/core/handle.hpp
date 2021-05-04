#pragma once
#include <twist/stdlike/mutex.hpp>
#include <mutex>
#include <wheels/support/intrusive_list.hpp>

namespace mtf::fibers {

class Awaiter;

// Lightweight non-owning handle to the fiber object
class FiberHandle {
 public:
  explicit FiberHandle(void* fiber) : fiber_(fiber) {
  }

  FiberHandle() : FiberHandle(nullptr) {
  }

  static FiberHandle Invalid() {
    return FiberHandle(nullptr);
  }

  static FiberHandle FromCurrent();

  bool IsValid() const {
    return fiber_ != nullptr;
  }

  void Suspend(Awaiter* awaiter);

  void Resume();

 private:
  void* fiber_;
};

class Awaiter : public wheels::IntrusiveListNode<Awaiter> {
 private:
  FiberHandle handle_;
  std::unique_lock<twist::stdlike::mutex>& mutex_;

 public:
  Awaiter(wheels::IntrusiveList<Awaiter>* futex,
          std::unique_lock<twist::stdlike::mutex>& mutex)
      : mutex_(mutex) {
    futex->PushBack(this);
  }
  void AwaitSuspend(FiberHandle handle) {
    handle_ = handle;
    mutex_.unlock();
  }
  void AwaitResume() {
    handle_.Resume();
  }
};

}  // namespace mtf::fibers
