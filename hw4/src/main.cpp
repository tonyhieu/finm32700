#include <iostream>
#include <ostream>

#include "OrderBookBuffer.hpp"
#include "StaticVector.hpp"
#include "constexpr_math.hpp"
#include "generic_algorithms.hpp"
#include "metaprogramming.hpp"

struct Order {
  int id;
  double price;
  int qty;

  friend std::ostream &operator<<(std::ostream &os, const Order &o) {
    return os << "Order(" << o.id << ", " << o.price << ", " << o.qty << ")\n";
  }
};

int main() {
  std::cout << "HFT Template Homework Examples\n";

  // Part 1: TMP + SFINAE + variadic templates.
  static_assert(Factorial<5>::value == 120, "Incorrect factorial");
  static_assert(Fibonacci<7>::value == 13, "Incorrect fibonacci");

  print_if_odd<11>();
  print_all("Order", 42, 101.75, true);

  // Part 2: constexpr math + static validation.
  static_assert(square(5) == 25, "Incorrect square");
  static_assert(price_bucket(101.73) == 101.70, "Incorrect price bucket");

  constexpr int Size = square(5);
  int arr[Size] = {};

  std::cout << "factorial_constexpr(5): " << factorial_constexpr(5) << '\n';
  std::cout << "fibonacci_constexpr(7): " << fibonacci_constexpr(7) << '\n';
  std::cout << "price_bucket(101.73): " << price_bucket(101.73) << '\n';
  std::cout << "Array size from constexpr square(5): " << std::size(arr)
            << '\n';

  // Part 3: StaticVector + generic find_if with Order.
  StaticVector<Order, 8> orders;
  orders.push_back({1, 99.95, 7});
  orders.push_back({2, 100.25, 12});
  orders.push_back({3, 101.10, 15});
  orders.push_back({4, 99.50, 20});

  auto first_price_gt_100 =
      find_if(orders.begin(), orders.end(),
              [](const Order &o) { return o.price > 100.0; });
  if (first_price_gt_100 != orders.end()) {
    std::cout << "First order with price > 100: id=" << first_price_gt_100->id
              << '\n';
  }

  auto first_qty_div_10 =
      find_if(orders.begin(), orders.end(),
              [](const Order &o) { return o.qty % 10 == 0; });
  if (first_qty_div_10 != orders.end()) {
    std::cout << "First order with qty divisible by 10: id="
              << first_qty_div_10->id << '\n';
  }

  // Part 4: Policy-based OrderBookBuffer usage.
  OrderBookBuffer<Order, StackAllocator, NoLock> book1(10);
  OrderBookBuffer<Order, HeapAllocator, MutexLock> book2(10);

  book1.add_order({10, 100.05, 10});
  book1.add_order({11, 100.20, 15});
  book2.add_order({20, 101.00, 25});

  book1.print_orders();
  book2.print_orders();

  return 0;
}
