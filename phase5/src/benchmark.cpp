#include "optimized_order_book.hpp"
#include "order_book.hpp"

#include <cassert>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

struct BenchmarkResult {
  int orderVolume;
  double elapsedSeconds;
  double throughputOpsPerSecond;
};

template <typename BookType>
BenchmarkResult runStressBenchmark(BookType &book, int numOrders,
                                   std::mt19937 &rng, bool unrollAddLoop) {
  std::uniform_real_distribution<double> priceDist(50.0, 150.0);
  std::uniform_int_distribution<int> quantityDist(1, 500);

  std::vector<std::string> ids;
  ids.reserve(static_cast<std::size_t>(numOrders));
  for (int i = 0; i < numOrders; ++i) {
    ids.push_back("ORD" + std::to_string(i));
  }

  book.clear();
  auto start = std::chrono::high_resolution_clock::now();

  if (unrollAddLoop) {
    int i = 0;
    for (; i + 1 < numOrders; i += 2) {
      const bool isBuy0 = (i % 2 == 0);
      const double price0 = priceDist(rng);
      const int quantity0 = quantityDist(rng);
      book.addOrder(ids[static_cast<std::size_t>(i)], price0, quantity0,
                    isBuy0);

      const int next = i + 1;
      const bool isBuy1 = (next % 2 == 0);
      const double price1 = priceDist(rng);
      const int quantity1 = quantityDist(rng);
      book.addOrder(ids[static_cast<std::size_t>(next)], price1, quantity1,
                    isBuy1);
    }

    for (; i < numOrders; ++i) {
      const bool isBuy = (i % 2 == 0);
      const double price = priceDist(rng);
      const int quantity = quantityDist(rng);
      book.addOrder(ids[static_cast<std::size_t>(i)], price, quantity, isBuy);
    }
  } else {
    for (int i = 0; i < numOrders; ++i) {
      const bool isBuy = (i % 2 == 0);
      const double price = priceDist(rng);
      const int quantity = quantityDist(rng);
      book.addOrder(ids[static_cast<std::size_t>(i)], price, quantity, isBuy);
    }
  }

  int modifyOps = 0;
  for (int i = 0; i < numOrders; i += 3) {
    const double newPrice = priceDist(rng);
    const int newQuantity = quantityDist(rng);
    if (book.modifyOrder(ids[static_cast<std::size_t>(i)], newPrice,
                         newQuantity)) {
      ++modifyOps;
    }
  }

  int deleteOps = 0;
  for (int i = 1; i < numOrders; i += 5) {
    if (book.deleteOrder(ids[static_cast<std::size_t>(i)])) {
      ++deleteOps;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed = end - start;
  const double totalOps =
      static_cast<double>(numOrders + modifyOps + deleteOps);
  const double throughput = totalOps / elapsed.count();

  return BenchmarkResult{numOrders, elapsed.count(), throughput};
}

void writeCsv(const std::vector<BenchmarkResult> &results,
              const std::string &filename) {
  std::ofstream out(filename);
  out << "order_volume,execution_time_seconds,throughput_ops_per_second\n";
  for (const auto &result : results) {
    out << result.orderVolume << ',' << std::fixed << std::setprecision(6)
        << result.elapsedSeconds << ',' << std::fixed << std::setprecision(2)
        << result.throughputOpsPerSecond << '\n';
  }
}

int main() {
  std::mt19937 rng(42);
  std::vector<int> orderVolumes{1000, 5000, 10000, 50000, 100000, 200000};

  // Benchmark unoptimized OrderBook
  {
    OrderBook book;
    std::vector<BenchmarkResult> results;
    results.reserve(orderVolumes.size());

    std::cout << "Unoptimized OrderBook stress test results\n";
    std::cout << "Orders\tTime (s)\tThroughput (ops/s)\n";
    for (int volume : orderVolumes) {
      rng.seed(42);
      BenchmarkResult result = runStressBenchmark(book, volume, rng, false);
      results.push_back(result);
      std::cout << result.orderVolume << '\t' << std::fixed
                << std::setprecision(6) << result.elapsedSeconds << '\t'
                << std::fixed << std::setprecision(2)
                << result.throughputOpsPerSecond << '\n';
    }
    writeCsv(results, "performance_data_unoptimized.csv");
    std::cout << "Saved unoptimized benchmark data to "
                 "performance_data_unoptimized.csv\n\n";
  }

  // Benchmark optimized OrderBook
  {
    OptimizedOrderBook book(250000);
    std::vector<BenchmarkResult> results;
    results.reserve(orderVolumes.size());

    std::cout << "Optimized OrderBook stress test results\n";
    std::cout << "Orders\tTime (s)\tThroughput (ops/s)\n";
    for (int volume : orderVolumes) {
      rng.seed(42);
      BenchmarkResult result = runStressBenchmark(book, volume, rng, true);
      results.push_back(result);
      std::cout << result.orderVolume << '\t' << std::fixed
                << std::setprecision(6) << result.elapsedSeconds << '\t'
                << std::fixed << std::setprecision(2)
                << result.throughputOpsPerSecond << '\n';
    }
    writeCsv(results, "performance_data_optimized.csv");
    std::cout
        << "Saved optimized benchmark data to performance_data_optimized.csv\n";
  }

  return 0;
}
