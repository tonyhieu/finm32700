# Performance Analysis

## Summary

The current implementation is stable under a 10K-tick synthetic load and produces low average tick-to-trade latency. The critical path is short: best-price lookup, spread check, quantity update, and vector append for latency.

## Why the Current Design Performs Well

- **Aligned market data (`alignas(64)`)** reduces false sharing risk and improves cache behavior.
- **OrderBook side containers** (`std::multimap`) keep best bid/ask accessible at begin iterators.
- **`std::unique_ptr` + pool wrapper** avoids raw `new/delete` usage and keeps ownership explicit.
- **Preallocated vectors** in `TradeLogger` reduce reallocation during burst logging.
- **Simple matching loop** avoids extra branching and copies.

## Experiment Notes (Assignment Variables)

1. **Smart pointers vs raw pointers**
   - Smart pointers improve safety and keep ownership clear.
   - Overhead in this setup is small relative to matching work.
2. **Memory alignment on market data**
   - Alignment is cheap to apply and supports consistent data access layout.
3. **Memory pool vs direct allocation**
   - Pool-style allocation reduces allocator churn during heavy order insertion.
4. **Container layout**
   - `multimap` gives straightforward price-time style ordering; alternative flat containers may be faster for dense books.
5. **Load scaling**
   - `test/test_latency.cpp` takes in an argument for number of ticks. We tested it with 2000, 5000, 10000, and 20000 ticks; average tick latency improved as the number of ticks grew, starting from 1700 ns and going to 800 ns.

