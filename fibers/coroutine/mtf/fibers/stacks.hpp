#pragma once

#include <context/stack.hpp>
#include <mutex>
#include <deque>

namespace mtf::fibers {

context::Stack AllocateStack();

class StackPool {
 public:
  StackPool() = delete;

  static context::Stack TakeStack();

  static void ReturnStack(context::Stack stack);

 private:
  static std::mutex mutex_;
  static std::deque<context::Stack> allocated_stacks_;  // guarded by mutex_
};
}  // namespace mtf::fibers
