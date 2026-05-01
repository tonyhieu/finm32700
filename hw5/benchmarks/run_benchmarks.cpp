#include "hw5/benchmarks.hpp"
#include "hw5/time_series_processor.hpp"

#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

double parse_speed_ratio(const std::string &label) {
  const std::string key = "speed_ratio=";
  const std::size_t start = label.find(key);
  if (start == std::string::npos) {
    return 0.0;
  }
  const std::size_t value_start = start + key.size();
  const std::size_t value_end = label.find('x', value_start);
  if (value_end == std::string::npos) {
    return 0.0;
  }
  return std::stod(label.substr(value_start, value_end - value_start));
}

std::vector<hw5::BookOrder> make_order_workload(std::size_t n) {
  std::mt19937_64 rng(123);
  std::uniform_real_distribution<double> price_dist(99.0, 101.0);

  std::vector<hw5::BookOrder> workload;
  workload.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    workload.push_back({static_cast<int>(i), price_dist(rng),
                        static_cast<int>(100 + (i % 5000)),
                        (i % 2 == 0) ? hw5::Side::Buy : hw5::Side::Sell});
  }
  return workload;
}

std::vector<double> make_prices(std::size_t n) {
  std::vector<double> prices(n);
  for (std::size_t i = 0; i < n; ++i) {
    prices[i] = 100.0 + static_cast<double>(i % 1000) * 0.01;
  }
  return prices;
}

} // namespace

int main() {
  const auto hash_stats = hw5::benchmark_hash_table_against_std(100000, 5);
  const auto pq_stats = hw5::benchmark_priority_queue_against_std(150000, 5);
  const auto ob_stats =
      hw5::benchmark_order_book_workload(make_order_workload(80000), 5);
  const auto ts_stats =
      hw5::benchmark_moving_average(make_prices(1200000), 32, 5);

  std::cout << std::fixed << std::setprecision(3);
  std::cout << "Homework 5 Benchmarks (Parts 1-4)\n\n";
  std::cout << "| Part | Custom Mean (ms) | Custom Median (ms) | "
               "Baseline/Custom Speed Ratio |\n";
  std::cout << "|---|---:|---:|---:|\n";
  std::cout << "| Part 1: Robin Hood Hash Table | " << hash_stats.mean_ms
            << " | " << hash_stats.median_ms << " | "
            << parse_speed_ratio(hash_stats.label) << "x |\n";
  std::cout << "| Part 2: Binary Heap Priority Queue | " << pq_stats.mean_ms
            << " | " << pq_stats.median_ms << " | "
            << parse_speed_ratio(pq_stats.label) << "x |\n";
  std::cout << "| Part 3: Time Series SIMD | " << ts_stats.simd_ms << " | "
            << ts_stats.simd_ms << " | "
            << ((ts_stats.simd_ms > 0.0)
                    ? (ts_stats.scalar_ms / ts_stats.simd_ms)
                    : 0.0)
            << "x |\n";
  std::cout << "| Part 4: Hybrid Order Book | " << ob_stats.mean_ms << " | "
            << ob_stats.median_ms << " | N/A |\n";

  std::cout << "\nPart 3 raw averages: scalar=" << ts_stats.scalar_ms
            << " ms, simd=" << ts_stats.simd_ms << " ms\n";
  std::cout << "Part 4 workload operations measured: " << ob_stats.operations
            << '\n';

  return 0;
}
