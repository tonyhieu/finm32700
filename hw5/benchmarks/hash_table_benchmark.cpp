#include "hw5/robin_hood_hash_table.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct ResultStats {
  double mean_ms = 0.0;
  double median_ms = 0.0;
};

std::vector<std::string> make_symbols(std::size_t n) {
  std::vector<std::string> symbols;
  symbols.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    symbols.push_back("SYM" + std::to_string(i));
  }
  return symbols;
}

ResultStats compute_stats(std::vector<double> samples_ms) {
  ResultStats stats;
  if (samples_ms.empty()) {
    return stats;
  }

  stats.mean_ms = std::accumulate(samples_ms.begin(), samples_ms.end(), 0.0) /
                  static_cast<double>(samples_ms.size());
  std::sort(samples_ms.begin(), samples_ms.end());
  const std::size_t mid = samples_ms.size() / 2;
  if (samples_ms.size() % 2 == 0) {
    stats.median_ms = (samples_ms[mid - 1] + samples_ms[mid]) / 2.0;
  } else {
    stats.median_ms = samples_ms[mid];
  }
  return stats;
}

ResultStats benchmark_robin_hood(const std::vector<std::string> &symbols,
                                 const std::vector<double> &prices,
                                 const std::vector<std::uint64_t> &volumes,
                                 const std::vector<std::size_t> &insert_order,
                                 const std::vector<std::size_t> &query_order,
                                 std::size_t iterations) {
  std::vector<double> times_ms;
  times_ms.reserve(iterations);

  volatile double sink = 0.0;
  for (std::size_t it = 0; it < iterations; ++it) {
    hw5::RobinHoodHashTable table(symbols.size() * 2 + 1);

    const auto start = std::chrono::steady_clock::now();
    for (std::size_t idx : insert_order) {
      table.insert(symbols[idx], {symbols[idx], prices[idx], volumes[idx]});
    }

    for (std::size_t idx : query_order) {
      const hw5::MarketData *found = table.find(symbols[idx]);
      if (found != nullptr) {
        sink += found->last_price;
      }
    }

    for (std::size_t idx : insert_order) {
      table.erase(symbols[idx]);
    }
    const auto end = std::chrono::steady_clock::now();
    const double elapsed_ms =
        std::chrono::duration<double, std::milli>(end - start).count();
    times_ms.push_back(elapsed_ms);
  }

  if (sink < 0.0) {
    std::cerr << "ignore " << sink << '\n';
  }
  return compute_stats(std::move(times_ms));
}

ResultStats benchmark_unordered_map(
    const std::vector<std::string> &symbols, const std::vector<double> &prices,
    const std::vector<std::uint64_t> &volumes,
    const std::vector<std::size_t> &insert_order,
    const std::vector<std::size_t> &query_order, std::size_t iterations) {
  std::vector<double> times_ms;
  times_ms.reserve(iterations);

  volatile double sink = 0.0;
  for (std::size_t it = 0; it < iterations; ++it) {
    std::unordered_map<std::string, hw5::MarketData> table;
    table.reserve(symbols.size() * 2 + 1);

    const auto start = std::chrono::steady_clock::now();
    for (std::size_t idx : insert_order) {
      table.emplace(symbols[idx],
                    hw5::MarketData{symbols[idx], prices[idx], volumes[idx]});
    }

    for (std::size_t idx : query_order) {
      auto found = table.find(symbols[idx]);
      if (found != table.end()) {
        sink += found->second.last_price;
      }
    }

    for (std::size_t idx : insert_order) {
      table.erase(symbols[idx]);
    }
    const auto end = std::chrono::steady_clock::now();
    const double elapsed_ms =
        std::chrono::duration<double, std::milli>(end - start).count();
    times_ms.push_back(elapsed_ms);
  }

  if (sink < 0.0) {
    std::cerr << "ignore " << sink << '\n';
  }
  return compute_stats(std::move(times_ms));
}

} // namespace

int main(int argc, char **argv) {
  std::size_t element_count = 100000;
  std::size_t iterations = 5;

  if (argc >= 2) {
    element_count = static_cast<std::size_t>(std::stoull(argv[1]));
  }
  if (argc >= 3) {
    iterations = static_cast<std::size_t>(std::stoull(argv[2]));
  }

  std::mt19937_64 rng(42);
  auto symbols = make_symbols(element_count);
  std::vector<double> prices(element_count);
  std::vector<std::uint64_t> volumes(element_count);
  for (std::size_t i = 0; i < element_count; ++i) {
    prices[i] = 100.0 + static_cast<double>(i % 1000) * 0.01;
    volumes[i] = 100 + (i % 5000);
  }

  std::vector<std::size_t> insert_order(element_count);
  std::iota(insert_order.begin(), insert_order.end(), 0);
  std::shuffle(insert_order.begin(), insert_order.end(), rng);

  std::vector<std::size_t> query_order = insert_order;
  std::shuffle(query_order.begin(), query_order.end(), rng);

  const ResultStats robin = benchmark_robin_hood(
      symbols, prices, volumes, insert_order, query_order, iterations);
  const ResultStats std_hash = benchmark_unordered_map(
      symbols, prices, volumes, insert_order, query_order, iterations);

  std::cout << std::fixed << std::setprecision(3);
  std::cout << "Hash Table Benchmark (insert + find + erase)\n";
  std::cout << "Elements: " << element_count << ", Iterations: " << iterations
            << "\n\n";
  std::cout << "| Implementation | Mean (ms) | Median (ms) |\n";
  std::cout << "|---|---:|---:|\n";
  std::cout << "| Robin Hood Hash Table | " << robin.mean_ms << " | "
            << robin.median_ms << " |\n";
  std::cout << "| std::unordered_map | " << std_hash.mean_ms << " | "
            << std_hash.median_ms << " |\n";

  if (robin.mean_ms > 0.0) {
    const double speed_ratio = std_hash.mean_ms / robin.mean_ms;
    std::cout << "\nSpeed ratio (std / robin): " << speed_ratio << "x\n";
  }

  return 0;
}
