#include "MarketData.hpp"

MarketDataFeed::MarketDataFeed(std::string symbol, double base_price, double tick_size)
    : symbol_(std::move(symbol)), base_price_(base_price), tick_size_(tick_size) {}

MarketData MarketDataFeed::nextTick(int tick_index) {
    const double drift = static_cast<double>(tick_index % 7 - 3) * tick_size_;
    const double mid = base_price_ + drift;
    return MarketData{
        symbol_,
        mid - 0.01,
        mid + 0.01,
        std::chrono::high_resolution_clock::now()};
}
