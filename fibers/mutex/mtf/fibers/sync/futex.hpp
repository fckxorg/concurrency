#pragma once

#include <mtf/fibers/core/handle.hpp>
#include <mtf/fibers/core/suspend.hpp>

#include <twist/stdlike/atomic.hpp>

#include <wheels/support/intrusive_list.hpp>

#include <queue>

namespace mtf::fibers {

// https://eli.thegreenplace.net/2018/basics-of-futexes/
class Awaiter : public wheels::IntrusiveListNode<Awaiter> {
 private:
  wheels::IntrusiveList<Awaiter>* queue_;
  FiberHandle* handle_;

 public:
  Awaiter(wheels::IntrusiveList<Awaiter>* futex) : queue_(futex) {
  }
  void AwaitSuspend(FiberHandle* handle) {
    handle_ = handle;
    queue_->PushBack(this);
    handle_->Suspend();
  }
  void AwaitResume() {
    handle_->Resume();
  }
};

template <typename T>
class FutexLike : public twist::stdlike::atomic<T>,
                  public wheels::IntrusiveList<Awaiter> {
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
    if (this->load() == old) {
      Awaiter awaiter{this};
      auto handle = FiberHandle::FromCurrent();
      awaiter.AwaitSuspend(&handle);
    }
  }

  void WakeOne() {
    if (this->Size()) {
      this->PopFront()->AwaitResume();
    }
  }

  void WakeAll() {
    for (auto size = this->Size(); size >= 0; --size) {
      this->PopFront()->AwaitResume();
    }
  }
};

}  // namespace mtf::fibers
