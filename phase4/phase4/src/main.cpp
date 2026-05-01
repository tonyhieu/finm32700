#include "MarketData.hpp"
#include "MatchingEngine.hpp"
#include "Order.hpp"
#include "OrderBook.hpp"
#include "OrderManager.hpp"
#include "TradeLogger.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

using PriceType = double;
using OrderIdType = int;
using OrderType = Order<PriceType, OrderIdType>;
using TradeType = MatchedTrade<PriceType, OrderIdType>;

struct LatencyStats {
    long long min_ns{0};
    long long max_ns{0};
    double mean_ns{0.0};
    double stddev_ns{0.0};
    long long p99_ns{0};
};

LatencyStats analyzeLatencies(const std::vector<long long>& latencies) {
    if (latencies.empty()) {
        return {};
    }

    const auto [min_it, max_it] = std::minmax_element(latencies.begin(), latencies.end());
    const double mean = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double variance = 0.0;
    for (const long long l : latencies) {
        const double d = static_cast<double>(l) - mean;
        variance += d * d;
    }
    variance /= latencies.size();

    std::vector<long long> sorted = latencies;
    std::sort(sorted.begin(), sorted.end());
    const std::size_t p99_index = static_cast<std::size_t>(0.99 * static_cast<double>(sorted.size() - 1));

    return LatencyStats{
        *min_it,
        *max_it,
        mean,
        std::sqrt(variance),
        sorted[p99_index]};
}

int main() {
    const int num_ticks = 10000;

    MarketDataFeed feed("AAPL", 150.0, 0.01);
    OrderBook<PriceType, OrderIdType> book(num_ticks);
    OrderManager<OrderIdType> manager;
    MatchingEngine<PriceType, OrderIdType> engine;
    TradeLogger logger(static_cast<std::size_t>(num_ticks));

    int next_id = 1;
    for (int i = 0; i < 500; ++i) {
        manager.registerNew(next_id);
        book.addOrder(OrderType(next_id++, "AAPL", 149.95 + (i % 5) * 0.01, 100, false));
    }

    std::size_t matched_count = 0;
    for (int i = 0; i < num_ticks; ++i) {
        const MarketData tick = feed.nextTick(i);
        const bool is_buy = (i % 2 == 0);
        const double price = is_buy ? tick.ask_price + 0.02 : tick.bid_price - 0.02;
        OrderType incoming(next_id++, tick.symbol, price, 100, is_buy);

        std::vector<TradeType> trades = engine.matchOrder(tick, std::move(incoming), book, manager);
        for (const auto& t : trades) {
            logger.addTrade(t);
        }
        matched_count += trades.size();

        if ((i + 1) % 1000 == 0) {
            logger.flushBatch();
        }
    }
    logger.flushBatch();
    logger.writeToFile("trades.csv");

    const auto stats = analyzeLatencies(engine.latencies());
    std::cout << "Tick-to-Trade Latency (nanoseconds)\n";
    std::cout << "Min: " << stats.min_ns << '\n';
    std::cout << "Max: " << stats.max_ns << '\n';
    std::cout << "Mean: " << stats.mean_ns << '\n';
    std::cout << "StdDev: " << stats.stddev_ns << '\n';
    std::cout << "P99: " << stats.p99_ns << '\n';
    std::cout << "Trades logged: " << logger.totalCount() << '\n';
    std::cout << "Match events: " << matched_count << '\n';
    return 0;
}
