# HFT Project (Phase 4)

This project is a modular C++ prototype for a high-frequency trading workflow:
- Simulate market data ticks
- Accept and manage orders
- Match orders in a limit order book
- Log trades in batches
- Measure tick-to-trade latency in nanoseconds

## Project Structure

```
hft_project/
├── include/
│   ├── MarketData.hpp
│   ├── Order.hpp
│   ├── OrderBook.hpp
│   ├── MatchingEngine.hpp
│   ├── OrderManager.hpp
│   ├── TradeLogger.hpp
│   └── Timer.hpp
├── src/
│   ├── MarketData.cpp
│   ├── OrderBook.cpp
│   ├── MatchingEngine.cpp
│   ├── OrderManager.cpp
│   ├── TradeLogger.cpp
│   └── main.cpp
├── test/
│   └── test_latency.cpp
├── docs/
│   ├── SystemArchitecture.md
│   ├── BenchmarkReport.md
│   └── PerformanceAnalysis.md
├── CMakeLists.txt
└── README.md
```

## Design Notes

- **MarketDataFeed** generates aligned ticks using `alignas(64)` market data.
- **OrderBook** uses a templated order type and stores order objects with `std::unique_ptr` in a simple memory pool.
- **OrderManager** tracks order lifecycle states: new, cancelled, partial, filled.
- **MatchingEngine** matches incoming orders against best opposite-side liquidity and records latency per match attempt.
- **TradeLogger** stores trades in vectors with `reserve()` and flushes in batches.
- **Timer** wraps `std::chrono::high_resolution_clock` for nanosecond measurement.

## Build and Run

```bash
cmake -S . -B build
cmake --build build
./build/hft_app
./build/test_latency
```

## Sample Output (Current Run)

From `hft_app`:
- Min latency: 375 ns
- Max latency: 57208 ns
- Mean latency: 582.934 ns
- StdDev: 661.509 ns
- P99 latency: 833 ns
- Trades logged: 5000

From `test_latency`:
- Latency samples: 2000
- Mean latency: 566.292 ns

## Deliverables

- Modular source code (`.hpp/.cpp`)
- README (design, architecture, build/run)
- Benchmark report (`docs/BenchmarkReport.md`)
- Performance test results + analysis (`docs/PerformanceAnalysis.md`)
- Architecture overview (`docs/SystemArchitecture.md`)
- Video demo: `demo.mov`
