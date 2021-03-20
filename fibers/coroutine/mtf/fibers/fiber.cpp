#include <mtf/fibers/api.hpp>

#include <mtf/coroutine/impl.hpp>
#include <mtf/fibers/stacks.hpp>

#include <cstdio>

namespace mtf::fibers {

using coroutine::impl::Coroutine;
using tp::StaticThreadPool;

////////////////////////////////////////////////////////////////////////////////

class Fiber {
 public:
  Fiber(Routine routine, StaticThreadPool& scheduler)
      : stack_(StackPool::TakeStack()),
        fiber_routine_(std::move(routine), stack_.View()) {
    worker_routine_ = [this]() {
      fiber_routine_.Resume();
      if (!fiber_routine_.IsCompleted()) {
        StaticThreadPool::Current()->Submit(worker_routine_);
      } else {
        delete this;
      }
    };
    scheduler.Submit(worker_routine_);
  }

  Fiber(const Fiber& other) = delete;

  ~Fiber() {
    StackPool::ReturnStack(std::move(stack_));
  }

  mtf::tp::Task GetWorkerRoutine() {
    return worker_routine_;
  }

 private:
  context::Stack stack_;
  Coroutine fiber_routine_;
  mtf::tp::Task worker_routine_;
};

////////////////////////////////////////////////////////////////////////////////

void Spawn(Routine routine, StaticThreadPool& scheduler) {
  new Fiber(std::move(routine), scheduler);
}

void Spawn(Routine routine) {
  Spawn(std::move(routine), *tp::Current());
}

void Yield() {
  Coroutine::Suspend();
}

}  // namespace mtf::fibers
