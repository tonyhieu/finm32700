# Homework 5 Write-up

To build:

```bash
cmake -S . -B build -DHW5_BUILD_STUBS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --parallel
```

## Benchmark Results

Run command:

```bash
./build/hw5_benchmarks
```

Observed output on this machine:

| Module | Custom Mean (ms) | Custom Median (ms) | Baseline/Custom Speed Ratio |
| --- | ---: | ---: | ---: |
| Robin Hood Hash Table | 59.962 | 59.143 | 0.948x |
| Binary Heap Priority Queue | 72.085 | 69.963 | 0.943x |
| Time Series SIMD | 18.049 | 18.049 | 1.000x |
| Hybrid Order Book | 81.229 | 80.781 | N/A |

- Time series raw averages: scalar `18.048 ms`, SIMD `18.049 ms`.

## Complexity Summary

- **Robin Hood Hash Table**
  - Insert: `O(1)` average, `O(n)` worst case
  - Delete: `O(1)` average, `O(n)` worst case
  - Find: `O(1)` average, `O(n)` worst case

- **Priority Queue (binary heap)**:
  - Insert: `O(log n)`
  - Extract top: `O(log n)`
  - Top lookup: `O(1)`

- **Time Series Moving Average**:
  - Scalar prefix-sum prep: `O(n)`
  - Window outputs: `O(n)`
  - Total: `O(n)`, with SIMD reducing constant factor on supported hardware.

- **Order Book**:
  - Add/modify/get/delete by ID: `O(1)` average for hash lookup + price-level vector maintenance.
  - Best bid/offer: `O(1)` from `map.begin()`.
  - Query by exact price level: `O(k)` where `k` is number of orders at that level.

## Trade-off Notes

- Robin Hood hashing has comparable performance to `std::unordered_map`'s bucket-based hashing. Because of the lack of pointers/linked lists, it uses less memory and has better cache locality. This comes at the cost of more expensive insertions (due to repeated swaps) which also leads to worse rehashing.
- Heap-based priority queue gives predictable `O(log n)` operations and direct control of ordering logic, but `std::priority_queue` can still edge it out due to STL-level tuning.
- SIMD path improves throughput for rolling analytics when instruction set support exists; fallback keeps behavior portable. However, the overhead of using SIMD can make small vectorizations less efficient.
- The hybrid order book design keeps best-price queries constant-time from ordered maps and keeps ID-driven updates fast via hash lookup, with the main cost concentrated in price-level vector compaction during deletions.
