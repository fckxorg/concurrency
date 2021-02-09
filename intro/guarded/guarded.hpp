#pragma once

#include <twist/stdlike/mutex.hpp>

namespace solutions {

// Automagically wraps all accesses to guarded object to critical sections
// Look at unit tests for API and usage examples

template <typename T>
class Unlocker {
 public:
  Unlocker(twist::stdlike::mutex& locked_mutex, T* object)
      : mutex_(locked_mutex), object_(object) {
  }

  ~Unlocker() {
    mutex_.unlock();
  }

  T* operator->() {
    return object_;
  }

 private:
  twist::stdlike::mutex& mutex_;  // Guards access to object_
  T* object_;
};

template <typename T>
class Guarded {
 public:
  // https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
  template <typename... Args>
  Guarded(Args&&... args) : object_(std::forward<Args>(args)...) {
  }

  // Your code goes here
  // https://en.cppreference.com/w/cpp/language/operators

  Unlocker<T> operator->() {
    mutex_.lock();
    return Unlocker<T>(mutex_, &object_);
  }

 private:
  T object_;
  twist::stdlike::mutex mutex_;  // Guards access to object_
};

}  // namespace solutions
