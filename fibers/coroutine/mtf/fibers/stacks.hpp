#pragma once

#include <context/stack.hpp>
#include <deque>
#include <mutex>

#include "spinlock.hpp"

namespace mtf::fibers {

context::Stack AllocateStack();

class StackPool {
 public:
  StackPool() = delete;

  static context::Stack TakeStack();

  static void ReturnStack(context::Stack stack);

 private:
  static mtf::spinlock::TATASSpinlock mutex;
  static std::deque<context::Stack> allocated_stacks;  // guarded by mutex_
};
}  // namespace mtf::fibers
