# Phase 3 - Local Order Book and Core Trading Infrastructure

This project builds a small local trading core in C++.

It does three things:
1. Tracks market bids and asks in a local snapshot
2. Tracks your active orders and their fill status
3. Runs a simple strategy loop over a text feed

## Files

- `market_snapshot.h/.cpp`: local best bid/ask storage
- `order_manager.h/.cpp`: order placement, fill handling, and cancellation
- `feed_parser.h/.cpp`: parser for `sample_feed.txt`
- `main.cpp`: strategy loop
- `sample_feed.txt`: simulated market and execution events

## Architecture

`main.cpp` reads feed events and sends each event to:
- `MarketSnapshot` for bid/ask updates
- `OrderManager` for execution updates

After each event, the strategy checks top-of-book and can place an order.

## Memory safety

- `MarketSnapshot` stores each price level in `std::unique_ptr`.
- `OrderManager` stores each active order in `std::unique_ptr`.
- Price levels are erased when quantity is 0.
- Orders are erased when fully filled or canceled.
- No manual `new` or `delete` is used.

## Build and run

From the `phase3` directory:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic *.cpp -o phase3_trader
./phase3_trader
```

Optional AddressSanitizer run:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -fsanitize=address -g *.cpp -o phase3_trader_asan
./phase3_trader_asan
```

## How to check correctness

- Confirm market output changes as bid/ask updates arrive
- Confirm orders get IDs, then move from new -> partial -> filled/removed
- Confirm fills for unknown IDs are ignored with a clear message

## How to check memory safety

Run the ASan build above. It should exit without memory errors.
