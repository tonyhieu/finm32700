#pragma once

constexpr int factorial_constexpr(int n) {
  if (n == 0)
    return 1;

  return n * factorial_constexpr(n - 1);
}

constexpr int fibonacci_constexpr(int n) {
  if (n <= 0)
    return 0;
  if (n == 1)
    return 1;

  return fibonacci_constexpr(n - 1) + fibonacci_constexpr(n - 2);
}

constexpr int square(int x) { return x * x; }

constexpr double price_bucket(double price) {
  return static_cast<int>(price / 0.05) * 0.05;
}

static_assert(square(5) == 25);
static_assert(price_bucket(101.73) == 101.70);
