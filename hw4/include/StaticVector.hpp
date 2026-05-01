#pragma once

#include <cstddef>
#include <stdexcept>

template <typename T, std::size_t N> class StaticVector {
public:
  using value_type = T;
  using size_type = std::size_t;
  using iterator = T *;

  T *array = nullptr;
  int curr_idx = 0;

  StaticVector() { array = new T[N]; }

  ~StaticVector() { delete[] array; }

  void push_back(const T &value) {
    if (curr_idx == N)
      throw std::runtime_error("Array is full");

    array[curr_idx] = value;
    ++curr_idx;
  }

  T &operator[](size_type index) {
    if (index >= N)
      throw std::invalid_argument("Index too large");

    return array[index];
  }

  const T &operator[](size_type index) const {
    if (index >= N)
      throw std::invalid_argument("Index too large");

    return array[index];
  }

  size_type size() const noexcept { return N; }

  iterator begin() noexcept { return array; }

  iterator end() noexcept { return array + N; }

private:
  T data_[N];
  size_type size_ = 0;
};
