#pragma once

#include <mtf/fibers/core/api.hpp>

#include <mtf/coroutine/impl.hpp>
#include <context/stack.hpp>

namespace mtf::fibers {

class Fiber {
 private:
  enum State { Suspended, Runnable, Running, Terminated };

 public:
  Fiber(Routine routine, Scheduler& scheduler);
  ~Fiber();

  static Fiber& AccessCurrent();

  // ~ System calls

  static void Spawn(Routine routine, Scheduler& scheduler);

  void Yield();
  void Suspend();  // Better API?
  void Resume();

 private:
  void Stop();
  void Step();
  void Schedule();
  void Reschedule();
  void Destroy();
  void Await();

 private:
  context::Stack stack_;
  mtf::coroutine::impl::Coroutine fiber_routine_;
  mtf::tp::Task worker_routine_;
  static Fiber* current_;
  Scheduler& sched_;
  State state_;
};

}  // namespace mtf::fibers
