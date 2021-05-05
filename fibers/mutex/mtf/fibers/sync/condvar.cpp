#include <mtf/fibers/sync/condvar.hpp>

namespace mtf::fibers {

void CondVar::Wait(Lock& lock) {
  uint32_t old = state_.load();

  n_waiting_.fetch_add(1);
  lock.unlock();
  state_.ParkIfEqual(old);
  lock.lock();
  n_waiting_.fetch_sub(1);
}

void CondVar::NotifyOne() {
  if (n_waiting_.load() != 0u) {
    state_.fetch_add(1);
    state_.WakeOne();
  }
}

void CondVar::NotifyAll() {
  if (n_waiting_.load() != 0u) {
    state_.fetch_add(1);
    state_.WakeAll();
  }
}

}  // namespace mtf::fibers
