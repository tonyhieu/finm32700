#include "hw5/benchmarks.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <numeric>
#include <queue>
#include <random>
#include <sstream>
#include <unordered_map>

namespace {

double median(std::vector<double> samples) {
  if (samples.empty()) {
    return 0.0;
  }
  std::sort(samples.begin(), samples.end());
  const std::size_t m = samples.size() / 2;
  if (samples.size() % 2 == 0) {
    return (samples[m - 1] + samples[m]) / 2.0;
  }
  return samples[m];
}

std::pair<double, double> summarize(const std::vector<double> &samples) {
  if (samples.empty()) {
    return {0.0, 0.0};
  }
  const double mean =
      std::accumulate(samples.begin(), samples.end(), 0.0) / samples.size();
  return {mean, median(samples)};
}

std::vector<std::string> make_symbols(std::size_t n) {
  std::vector<std::string> symbols;
  symbols.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    symbols.push_back("SYM" + std::to_string(i));
  }
  return symbols;
}

} // namespace

hw5::BenchmarkStats
hw5::benchmark_hash_table_against_std(std::size_t element_count,
                                      std::size_t iterations) {
  std::mt19937_64 rng(42);
  auto symbols = make_symbols(element_count);
  std::vector<std::size_t> order(element_count);
  std::iota(order.begin(), order.end(), 0);
  std::shuffle(order.begin(), order.end(), rng);

  std::vector<double> robin_ms;
  std::vector<double> std_ms;
  robin_ms.reserve(iterations);
  std_ms.reserve(iterations);

  volatile double sink = 0.0;
  for (std::size_t it = 0; it < iterations; ++it) {
    {
      RobinHoodHashTable table(element_count * 2 + 1);
      const auto start = std::chrono::steady_clock::now();
      for (std::size_t i : order) {
        table.insert(symbols[i], {symbols[i], 100.0 + (i % 1000) * 0.01, i});
      }
      for (std::size_t i : order) {
        const MarketData *md = table.find(symbols[i]);
        if (md != nullptr) {
          sink += md->last_price;
        }
      }
      for (std::size_t i : order) {
        table.erase(symbols[i]);
      }
      const auto end = std::chrono::steady_clock::now();
      robin_ms.push_back(
          std::chrono::duration<double, std::milli>(end - start).count());
    }
    {
      std::unordered_map<std::string, MarketData> table;
      table.reserve(element_count * 2 + 1);
      const auto start = std::chrono::steady_clock::now();
      for (std::size_t i : order) {
        table.emplace(symbols[i],
                      MarketData{symbols[i], 100.0 + (i % 1000) * 0.01, i});
      }
      for (std::size_t i : order) {
        auto found = table.find(symbols[i]);
        if (found != table.end()) {
          sink += found->second.last_price;
        }
      }
      for (std::size_t i : order) {
        table.erase(symbols[i]);
      }
      const auto end = std::chrono::steady_clock::now();
      std_ms.push_back(
          std::chrono::duration<double, std::milli>(end - start).count());
    }
  }

  const auto [robin_mean, robin_median] = summarize(robin_ms);
  const auto [std_mean, _std_median] = summarize(std_ms);
  std::ostringstream label;
  label << "robin_hood_vs_std(speed_ratio=" << (std_mean / robin_mean) << "x)";
  if (sink < 0.0) {
    sink = 0.0;
  }
  return {label.str(), robin_mean, robin_median,
          element_count * 3 * iterations};
}

hw5::BenchmarkStats
hw5::benchmark_priority_queue_against_std(std::size_t element_count,
                                          std::size_t iterations) {
  std::mt19937_64 rng(7);
  std::uniform_real_distribution<double> px_dist(99.0, 101.0);
  std::vector<Order> orders;
  orders.reserve(element_count);
  for (std::size_t i = 0; i < element_count; ++i) {
    orders.push_back(
        {static_cast<int>(i), px_dist(rng), static_cast<int>(100 + i % 5000),
         static_cast<std::int64_t>(i), (i % 2 == 0 ? 'B' : 'S')});
  }

  std::vector<double> custom_ms;
  std::vector<double> std_ms;
  custom_ms.reserve(iterations);
  std_ms.reserve(iterations);
  volatile double sink = 0.0;

  for (std::size_t it = 0; it < iterations; ++it) {
    {
      OrderPriorityQueue pq(HeapType::Max);
      const auto start = std::chrono::steady_clock::now();
      for (const auto &order : orders) {
        pq.push(order);
      }
      while (!pq.empty()) {
        sink += pq.pop().price;
      }
      const auto end = std::chrono::steady_clock::now();
      custom_ms.push_back(
          std::chrono::duration<double, std::milli>(end - start).count());
    }
    {
      std::priority_queue<Order> pq;
      const auto start = std::chrono::steady_clock::now();
      for (const auto &order : orders) {
        pq.push(order);
      }
      while (!pq.empty()) {
        sink += pq.top().price;
        pq.pop();
      }
      const auto end = std::chrono::steady_clock::now();
      std_ms.push_back(
          std::chrono::duration<double, std::milli>(end - start).count());
    }
  }

  const auto [custom_mean, custom_median] = summarize(custom_ms);
  const auto [std_mean, _std_median] = summarize(std_ms);
  std::ostringstream label;
  label << "binary_heap_vs_std_priority_queue(speed_ratio="
        << (std_mean / custom_mean) << "x)";
  if (sink < 0.0) {
    sink = 0.0;
  }
  return {label.str(), custom_mean, custom_median,
          element_count * 2 * iterations};
}

hw5::BenchmarkStats
hw5::benchmark_order_book_workload(const std::vector<BookOrder> &workload,
                                   std::size_t iterations) {
  if (workload.empty() || iterations == 0) {
    return {"order_book_efficiency", 0.0, 0.0, 0};
  }

  std::vector<double> custom_ms;
  custom_ms.reserve(iterations);
  volatile double sink = 0.0;

  for (std::size_t it = 0; it < iterations; ++it) {
    OrderBook book;
    const auto start = std::chrono::steady_clock::now();
    for (const auto &order : workload) {
      book.add_order(order);
    }
    for (std::size_t i = 0; i < workload.size(); i += 3) {
      book.modify_order(workload[i].id, workload[i].quantity + 10);
    }
    for (std::size_t i = 0; i < workload.size(); i += 10) {
      const auto matches =
          book.query_price_level(workload[i].price, workload[i].side);
      sink += static_cast<double>(matches.size());
    }
    if (auto bid = book.best_bid(); bid.has_value()) {
      sink += bid.value();
    }
    if (auto ask = book.best_offer(); ask.has_value()) {
      sink += ask.value();
    }
    for (std::size_t i = 0; i < workload.size(); i += 2) {
      book.delete_order(workload[i].id);
    }
    const auto end = std::chrono::steady_clock::now();
    custom_ms.push_back(
        std::chrono::duration<double, std::milli>(end - start).count());
  }

  const auto [custom_mean, custom_median] = summarize(custom_ms);
  if (sink < 0.0) {
    sink = 0.0;
  }
  return {"order_book_efficiency", custom_mean, custom_median,
          workload.size() * 4 * iterations};
}
