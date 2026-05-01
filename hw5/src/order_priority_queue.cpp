#include "hw5/order_priority_queue.hpp"

#include <stdexcept>
#include <utility>

hw5::OrderPriorityQueue::OrderPriorityQueue(HeapType heap_type)
    : heap_type_(heap_type) {}

void hw5::OrderPriorityQueue::push(const Order &order) {
  heap_.push_back(order);
  sift_up(heap_.size() - 1);
}

hw5::Order hw5::OrderPriorityQueue::pop() {
  if (heap_.empty()) {
    throw std::out_of_range("pop() on empty OrderPriorityQueue");
  }

  Order root = heap_.front();
  heap_.front() = heap_.back();
  heap_.pop_back();
  if (!heap_.empty()) {
    sift_down(0);
  }
  return root;
}

const hw5::Order &hw5::OrderPriorityQueue::top() const {
  if (heap_.empty()) {
    throw std::out_of_range("top() on empty OrderPriorityQueue");
  }
  return heap_.front();
}

bool hw5::OrderPriorityQueue::empty() const noexcept { return heap_.empty(); }

std::size_t hw5::OrderPriorityQueue::size() const noexcept { return heap_.size(); }

bool hw5::OrderPriorityQueue::compare(const Order &lhs, const Order &rhs) const {
  if (heap_type_ == HeapType::Max) {
    if (lhs.price != rhs.price) {
      return lhs.price > rhs.price;
    }
  } else {
    if (lhs.price != rhs.price) {
      return lhs.price < rhs.price;
    }
  }

  if (lhs.timestamp != rhs.timestamp) {
    return lhs.timestamp < rhs.timestamp;
  }
  return lhs.id < rhs.id;
}

void hw5::OrderPriorityQueue::sift_up(std::size_t index) {
  while (index > 0) {
    const std::size_t parent = (index - 1) / 2;
    if (!compare(heap_[index], heap_[parent])) {
      break;
    }
    std::swap(heap_[index], heap_[parent]);
    index = parent;
  }
}

void hw5::OrderPriorityQueue::sift_down(std::size_t index) {
  while (true) {
    const std::size_t left = 2 * index + 1;
    const std::size_t right = 2 * index + 2;
    std::size_t best = index;

    if (left < heap_.size() && compare(heap_[left], heap_[best])) {
      best = left;
    }
    if (right < heap_.size() && compare(heap_[right], heap_[best])) {
      best = right;
    }
    if (best == index) {
      break;
    }

    std::swap(heap_[index], heap_[best]);
    index = best;
  }
}
