#include "optimized_order_book.hpp"
#include "order_book.hpp"

#include <cassert>
#include <iostream>

void testAddOrder() {
  OrderBook book;
  assert(book.addOrder("ORD001", 100.25, 50, true));
  assert(book.hasOrder("ORD001"));
  assert(book.orderCount() == 1);
  assert(!book.addOrder("ORD001", 100.30, 10, true));

  auto [hasBid, bidPrice] = book.bestBid();
  assert(hasBid);
  assert(bidPrice == 100.25);
}

void testModifyOrder() {
  OrderBook book;
  assert(book.addOrder("ORD002", 99.50, 100, false));
  assert(book.modifyOrder("ORD002", 98.75, 80));
  assert(book.hasOrder("ORD002"));
  assert(book.askLevels() == 1);

  auto [hasAsk, askPrice] = book.bestAsk();
  assert(hasAsk);
  assert(askPrice == 98.75);
  assert(!book.modifyOrder("MISSING", 100.00, 1));
}

void testDeleteOrder() {
  OrderBook book;
  assert(book.addOrder("ORD003", 101.10, 25, true));
  assert(book.deleteOrder("ORD003"));
  assert(!book.hasOrder("ORD003"));
  assert(book.orderCount() == 0);
  assert(book.bidLevels() == 0);
  assert(!book.deleteOrder("ORD003"));
}

void testOptimizedAddOrder() {
  OptimizedOrderBook book;
  assert(book.addOrder("ORD001", 100.25, 50, true));
  assert(book.hasOrder("ORD001"));
  assert(book.orderCount() == 1);
  assert(!book.addOrder("ORD001", 100.30, 10, true));

  auto [hasBid, bidPrice] = book.bestBid();
  assert(hasBid);
  assert(bidPrice == 100.25);
}

void testOptimizedModifyOrder() {
  OptimizedOrderBook book;
  assert(book.addOrder("ORD002", 99.50, 100, false));
  assert(book.modifyOrder("ORD002", 98.75, 80));
  assert(book.hasOrder("ORD002"));
  assert(book.askLevels() == 1);

  auto [hasAsk, askPrice] = book.bestAsk();
  assert(hasAsk);
  assert(askPrice == 98.75);
  assert(!book.modifyOrder("MISSING", 100.00, 1));
}

void testOptimizedDeleteOrder() {
  OptimizedOrderBook book;
  assert(book.addOrder("ORD003", 101.10, 25, true));
  assert(book.deleteOrder("ORD003"));
  assert(!book.hasOrder("ORD003"));
  assert(book.orderCount() == 0);
  assert(book.bidLevels() == 0);
  assert(!book.deleteOrder("ORD003"));
}

void runUnitTests() {
  testAddOrder();
  testModifyOrder();
  testDeleteOrder();
  testOptimizedAddOrder();
  testOptimizedModifyOrder();
  testOptimizedDeleteOrder();
  std::cout << "Unit tests: passed\n";
}

int main() { runUnitTests(); }
