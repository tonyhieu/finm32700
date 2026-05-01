# Benchmark Report: Tick-to-Trade Latency

## Test Setup

- Build: CMake, C++17
- Main workload: `10000` ticks
- Initial liquidity seeded in order book: `500` sell orders
- Incoming flow: alternating buy/sell orders
- Latency unit: nanoseconds (`std::chrono::high_resolution_clock`)

## Measurement Logic

- Start timer when processing each incoming order in `MatchingEngine::matchOrder`
- Run matching logic and optional residual book insert
- Stop timer and push latency to `std::vector<long long>`

## Results (hft_app)

| Metric | Value |
| --- | --- |
| Min | 375 ns |
| Max | 57208 ns |
| Mean | 582.934 ns |
| StdDev | 661.509 ns |
| P99 | 833 ns |
| Trades logged | 5000 |
| Match events | 5000 |

