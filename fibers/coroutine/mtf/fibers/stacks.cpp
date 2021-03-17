#include <mtf/fibers/stacks.hpp>

namespace mtf::fibers {

using context::Stack;

Stack AllocateStack() {
  static const size_t kStackPages = 8;
  return Stack::AllocatePages(kStackPages);
}

context::Stack StackPool::TakeStack() {
  std::lock_guard lock(mutex_);

  if (allocated_stacks_.empty()) {
    return AllocateStack();
  } else {
    context::Stack free_stack = std::move(allocated_stacks_.front());
    allocated_stacks_.pop_front();
    return free_stack;
  }
}

void StackPool::ReturnStack(context::Stack stack) {
  std::lock_guard lock(mutex_);

  allocated_stacks_.push_back(std::move(stack));
}

mtf::spinlock::TATASSpinlock StackPool::mutex_{};
std::deque<context::Stack> StackPool::allocated_stacks_{};

}  // namespace mtf::fibers
