#pragma once

#include <cstddef>
#include <iostream>
#include <mutex>
#include <stdexcept>

template <typename T> class HeapAllocator {
public:
  T *allocate(std::size_t count) { return new T[count]; }

  void deallocate(T *ptr) { delete[] ptr; }
};

template <typename T> class StackAllocator {
  T *stack = nullptr;
  std::size_t top = 0, size;

public:
  explicit StackAllocator() : size(1024) { stack = new T[1024]; }

  ~StackAllocator() { delete[] stack; }

  T *allocate(std::size_t count) {
    if (count + top > size)
      throw std::runtime_error("Stack too full");

    top += count;
    return stack + (top - count);
  }

  void deallocate(T *ptr); // no-op
};

class MutexLock {
  std::mutex m;

public:
  void lock() { m.lock(); }
  void unlock() { m.unlock(); }
};

class NoLock {
public:
  void lock() {}   // no-op
  void unlock() {} // no-op
};

template <typename T, template <typename> class AllocatorPolicy,
          typename ThreadingPolicy>
class OrderBookBuffer {
public:
  explicit OrderBookBuffer(std::size_t capacity) : capacity_(capacity) {
    data_ = allocator_.allocate(capacity);
  }

  void add_order(const T &order) {
    lock_policy_.lock();

    if (size_ < capacity_)
      data_[size_++] = order;

    lock_policy_.unlock();
  }

  void print_orders() {
    lock_policy_.lock();

    for (auto i = 0; i < size_; ++i)
      std::cout << data_[i] << '\n';

    lock_policy_.unlock();
  }

private:
  std::size_t capacity_ = 0;
  std::size_t size_ = 0;
  AllocatorPolicy<T> allocator_;
  ThreadingPolicy lock_policy_;
  T *data_ = nullptr;
};
