# System Architecture Overview

## Module Flow

1. **MarketDataFeed**
   - Produces ticks with symbol, bid/ask, and timestamp.
2. **OrderManager**
   - Registers new orders and updates order state after matching.
3. **OrderBook**
   - Stores buy/sell orders and exposes best opposite-side liquidity.
4. **MatchingEngine**
   - Takes a tick + incoming order, checks spread crossing, and emits matched trades.
5. **TradeLogger**
   - Buffers matched trades and flushes them in batches to persistent storage (CSV).
6. **Latency Analyzer (main/test)**
   - Reads collected nanosecond latencies and reports min/max/mean/stddev/p99.

## Data Path (Text)

`MarketDataFeed -> MatchingEngine -> OrderBook (read/write) -> TradeLogger -> trades.csv`

`MatchingEngine -> latency vector -> analysis output`
