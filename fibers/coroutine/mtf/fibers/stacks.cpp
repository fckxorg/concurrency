#include <mtf/fibers/stacks.hpp>

namespace mtf::fibers {

using context::Stack;

Stack AllocateStack() {
  static const size_t kStackPages = 8;
  return Stack::AllocatePages(kStackPages);
}

context::Stack StackPool::TakeStack() {
  std::lock_guard lock(mutex);

  if (allocated_stacks.empty()) {
    return AllocateStack();
  } else {
    context::Stack free_stack = std::move(allocated_stacks.front());
    allocated_stacks.pop_front();
    return free_stack;
  }
}

void StackPool::ReturnStack(context::Stack stack) {
  std::lock_guard lock(mutex);

  allocated_stacks.push_back(std::move(stack));
}

mtf::spinlock::TATASSpinlock StackPool::mutex{};
std::deque<context::Stack> StackPool::allocated_stacks{};

}  // namespace mtf::fibers
