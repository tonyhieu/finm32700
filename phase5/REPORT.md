# Phase 5 Report: HFT Order Book

I implemented two order book classes: a baseline `OrderBook` and an optimized `OptimizedOrderBook` with memory pre-allocation and loop unrolling. Both were benchmarked under randomized load.

## Implementation Summary

### OrderBook (Baseline)
- Core structure:
  - `std::map<double, std::unordered_map<std::string, Order>, std::greater<double>>` for bid levels
  - `std::map<double, std::unordered_map<std::string, Order>>` for ask levels
  - `std::unordered_map<std::string, Order>` for O(1)-average ID lookup
- Operations: `addOrder`, `modifyOrder`, `deleteOrder`

### OptimizedOrderBook (Step 3 Optimizations)
- Memory pre-allocation:
  - Constructor pre-reserves `expectedOrders` capacity
  - `orderPool_` vector avoids fragmentation, enables sequential cache access
  - `active_` bit-vector tracks slot occupation
  - `freeIndices_` stack recycles deleted slots
- Loop unrolling:
  - Add operation processes two orders per iteration when unrollAddLoop=true
  - Reduces loop overhead and improves branch prediction
- Hash table tuning:
  - Set load factor to 0.70 to reduce collision cost
- All operations use index-based lookup instead of copying Order structs

## Unit Test Results

All unit tests passed for both `OrderBook` and `OptimizedOrderBook`:

```
Unit tests: passed
```

## Stress Test Setup

- Random order flow with fixed seed (`std::mt19937(42)`) for reproducibility
- Price range: `[50.0, 150.0]`
- Quantity range: `[1, 500]`
- For each volume `N`:
  - Add `N` orders
  - Modify every 3rd order
  - Delete every 5th order
- Timing tool: `std::chrono::high_resolution_clock`
- Compiler mode used for final chart: `-O3`

## Execution Time Results (-O3, OptimizedOrderBook with loop unrolling)

| Order Volume | Execution Time (s) | Throughput (ops/s) | Avg Latency per Order (µs) |
| --- | ---: | ---: | ---: |
| 1,000 | 0.000297 | 5,162,810.27 | 0.2970 |
| 5,000 | 0.001803 | 4,253,145.06 | 0.3606 |
| 10,000 | 0.003373 | 4,546,775.39 | 0.3373 |
| 50,000 | 0.026461 | 2,897,353.89 | 0.5292 |
| 100,000 | 0.057480 | 2,667,611.93 | 0.5748 |
| 200,000 | 0.172453 | 1,778,263.79 | 0.8623 |

The optimized implementation sustained over 1.7 million operations per second at the largest tested load.

## Compiler Optimization Effectiveness (-O0 vs -O3)

| Order Volume | O0 Time (s) | O3 Time (s) | Speedup |
| --- | ---: | ---: | ---: |
| 1,000 | 0.002539 | 0.000297 | 8.55x |
| 5,000 | 0.011711 | 0.001803 | 6.50x |
| 10,000 | 0.021562 | 0.003373 | 6.39x |
| 50,000 | 0.083720 | 0.026461 | 3.16x |
| 100,000 | 0.182311 | 0.057480 | 3.17x |
| 200,000 | 0.429828 | 0.172453 | 2.49x |

Compiler optimization provided a runtime reduction of 2.49x to 8.55x on the optimized code.

## Latency Breakdown

- Sub-microsecond average latency up to 10k orders
- Around 0.5-0.8 microseconds per order at 50k-200k volumes
- Memory pool and pre-allocation reduce fragmentation-induced latency variance
- Loop unrolling improves throughput by reducing iteration overhead

## Performance Chart

Generated from measured benchmark data:

![Performance chart](performance_chart.png)

## Files Submitted

- `order_book.cpp` (OrderBook baseline + OptimizedOrderBook with memory pooling and loop unrolling, unit tests, templated benchmark, CSV writer)
- `plot_performance.py` (chart generation)
- `performance_data.csv` (final `-O3` OptimizedOrderBook benchmark data)
- `performance_data_O0.csv` and `performance_data_O3.csv` (O0 vs O3 comparison for OptimizedOrderBook)
- `performance_chart.png` (execution time + throughput visualization)
- `run_O0.txt` and `run_O3.txt` (captured run logs for OptimizedOrderBook)
