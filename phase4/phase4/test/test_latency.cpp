#include "MarketData.hpp"
#include "MatchingEngine.hpp"
#include "Order.hpp"
#include "OrderBook.hpp"
#include "OrderManager.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numeric>

int main(int argc, char** argv) {
  using PriceType = double;
  using OrderIdType = int;
  using OrderType = Order<PriceType, OrderIdType>;

  int ticks = 2000;
  if (argc > 1) {
    ticks = std::atoi(argv[1]);
    if (ticks <= 0) {
      std::cerr << "Tick count must be a positive integer.\n";
      return 1;
    }
  }

  MarketDataFeed feed("AAPL", 100.0, 0.01);
  OrderBook<PriceType, OrderIdType> book(static_cast<std::size_t>(ticks + 500));
  OrderManager<OrderIdType> manager;
  MatchingEngine<PriceType, OrderIdType> engine;

  int id = 1;
  for (int i = 0; i < 500; ++i) {
    manager.registerNew(id);
    book.addOrder(OrderType(id++, "AAPL", 100.0 + (i % 3) * 0.01, 50, false));
  }

  for (int i = 0; i < ticks; ++i) {
    const auto tick = feed.nextTick(i);
    OrderType incoming(id++, "AAPL", tick.ask_price + 0.02, 50, true);
    (void)engine.matchOrder(tick, std::move(incoming), book, manager);
  }

  const auto &latencies = engine.latencies();
  assert(!latencies.empty());
  const double mean = std::accumulate(latencies.begin(), latencies.end(), 0.0) /
                      latencies.size();
  assert(mean >= 0.0);
  std::cout << "Latency samples: " << latencies.size() << '\n';
  std::cout << "Mean latency (ns): " << mean << '\n';
  return 0;
}
