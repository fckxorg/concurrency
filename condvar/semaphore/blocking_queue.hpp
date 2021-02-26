#pragma once

#include "semaphore.hpp"

#include <deque>

namespace solutions {

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BlockingQueue {
 public:
  explicit BlockingQueue(size_t capacity)
      : semaphore_empty_(capacity),
        semaphore_elements_(0),
        semaphore_mutex_(1) {
  }

  // Inserts the specified element into this queue
  void Put(T element) {
    semaphore_empty_.Acquire();
    semaphore_mutex_.Acquire();

    queue_.push_front(std::move(element));

    semaphore_mutex_.Release();
    semaphore_elements_.Release();
  }

  // Retrieves and removes the head of this queue,
  // waiting if necessary until an element becomes available
  T Take() {
    semaphore_elements_.Acquire();
    semaphore_mutex_.Acquire();

    T element = std::move(queue_.back());
    queue_.pop_back();

    semaphore_mutex_.Release();
    semaphore_empty_.Release();

    return element;
  }

 private:
  std::deque<T> queue_;
  Semaphore semaphore_empty_;
  Semaphore semaphore_elements_;
  Semaphore semaphore_mutex_;
};

}  // namespace solutions
