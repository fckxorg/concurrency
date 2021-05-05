#include <mtf/fibers/core/fiber.hpp>

#include <mtf/fibers/core/stacks.hpp>
#include <mtf/fibers/core/handle.hpp>

#include <wheels/support/assert.hpp>
#include <wheels/support/exception.hpp>

namespace mtf::fibers {

Fiber* Fiber::current_{nullptr};

Fiber& Fiber::AccessCurrent() {
  return *current_;
}

Fiber::Fiber(Routine routine, Scheduler& scheduler)
    : stack_(StackPool::TakeStack()),
      fiber_routine_(std::move(routine), stack_.View()),
      sched_(scheduler) {
  Resume();
}

Fiber::~Fiber() {
  StackPool::ReturnStack(std::move(stack_));
}

void Fiber::Spawn(Routine routine, Scheduler& scheduler) {
  new Fiber(std::move(routine), scheduler);
}

// System calls

void Fiber::Yield() {
  mtf::coroutine::impl::Coroutine::Suspend();
}

void Fiber::Suspend(Awaiter* awaiter) {
  state_ = Suspended;
  awaiter_ = awaiter;
  Yield();
}

void Fiber::Resume() {
  if (awaiter_) {
    awaiter_->AwaitResume();
  }

  awaiter_ = nullptr;
  state_ = Running;
  Reschedule();
}

// Scheduler ops

void Fiber::Schedule() {
  sched_.Submit([this]() {
    current_ = this;
    state_ = Running;
    fiber_routine_.Resume();
    Reschedule();
  });
}

void Fiber::Reschedule() {
  if (fiber_routine_.IsCompleted()) {
    state_ = Terminated;
    Destroy();
  } else {
    if (state_ == Running) {
      Step();
    }

    if (state_ == Suspended) {
      if (awaiter_) {
        awaiter_->AwaitSuspend(FiberHandle(this));
        awaiter_ = nullptr;
      }
    }
  }
}

/*
void Fiber::Await() {
}
*/

void Fiber::Destroy() {
  delete this;
}

void Fiber::Step() {
  state_ = Runnable;
  Schedule();
}

void Fiber::Stop() {
  // Not implemented
}

}  // namespace mtf::fibers
