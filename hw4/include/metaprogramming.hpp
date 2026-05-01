#pragma once

#include <iostream>
#include <type_traits>
#include <utility>

template <int N> struct Factorial;

template <> struct Factorial<0> {
  static constexpr int value = 1;
};

template <int N> struct Factorial {
  static constexpr int value = N * Factorial<N - 1>::value;
};

static_assert(Factorial<5>::value == 120);

template <int N> struct Fibonacci;

template <> struct Fibonacci<0> {
  static constexpr int value = 0;
};

template <> struct Fibonacci<1> {
  static constexpr int value = 1;
};

template <int N> struct Fibonacci {
  static constexpr int value =
      Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

static_assert(Fibonacci<7>::value == 13);

template <auto N> struct IsOdd {
  static constexpr bool value = (N % 2 != 0);
};

template <auto N, typename std::enable_if<IsOdd<N>::value, int>::type = 0>
void print_if_odd() {
  std::cout << N << " is odd\n";
}

template <typename... Args> void print_all(Args... args) {
  (std::cout << ... << args) << '\n';
}
