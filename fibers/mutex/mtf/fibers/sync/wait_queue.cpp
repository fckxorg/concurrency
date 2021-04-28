#include <mtf/fibers/sync/wait_queue.hpp>
#include <mtf/fibers/core/fiber.hpp>
#include <unordered_map>
#include <queue>

namespace mtf::fibers {
static std::unordered_map<void*, std::queue<Fiber*>>
    awaiting_;  // use address of FutexLike inside this map

void WaitQueue::Park(void* ptr) {
  Fiber* current = &(Fiber::AccessCurrent());
  awaiting_[ptr].push(current);
  current->Suspend();
}

size_t WaitQueue::Size(void* ptr) {
  return awaiting_[ptr].size();
}
void WaitQueue::WakeOne(void* ptr) {
  awaiting_[ptr].front()->Resume();
  awaiting_[ptr].pop();
}

void WaitQueue::WakeAll(void* ptr) {
  while (Size(ptr)) {
    WakeOne(ptr);
  }
}
}  // namespace mtf::fibers
