#include "TradeLogger.hpp"

TradeLogger::TradeLogger(std::size_t reserve_count) {
    pending_.reserve(reserve_count);
    persisted_.reserve(reserve_count);
}

void TradeLogger::flushBatch() {
    persisted_.insert(persisted_.end(), pending_.begin(), pending_.end());
    pending_.clear();
}

void TradeLogger::writeToFile(const std::string& path) const {
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    for (const auto& trade : persisted_) {
        out << trade.buy_order_id << ','
            << trade.sell_order_id << ','
            << trade.price << ','
            << trade.quantity << '\n';
    }
}

std::size_t TradeLogger::pendingCount() const {
    return pending_.size();
}

std::size_t TradeLogger::totalCount() const {
    return persisted_.size();
}
