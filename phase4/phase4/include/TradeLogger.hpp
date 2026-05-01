#pragma once

#include "MatchingEngine.hpp"

#include <fstream>
#include <string>
#include <vector>

struct TradeLogEntry {
    long long buy_order_id{0};
    long long sell_order_id{0};
    double price{0.0};
    int quantity{0};
};

class TradeLogger {
public:
    explicit TradeLogger(std::size_t reserve_count = 0);

    template <typename PriceType, typename OrderIdType>
    void addTrade(const MatchedTrade<PriceType, OrderIdType>& trade) {
        TradeLogEntry entry{
            static_cast<long long>(trade.buy_order_id),
            static_cast<long long>(trade.sell_order_id),
            static_cast<double>(trade.price),
            trade.quantity};
        pending_.push_back(entry);
    }

    void flushBatch();
    void writeToFile(const std::string& path) const;
    std::size_t pendingCount() const;
    std::size_t totalCount() const;

private:
    std::vector<TradeLogEntry> pending_;
    std::vector<TradeLogEntry> persisted_;
};
