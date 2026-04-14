#include "market_snapshot.h"

void MarketSnapshot::update_bid(double price, int qty) {
    if (qty <= 0) {
        bids_.erase(price);
        return;
    }
    bids_[price] = std::make_unique<PriceLevel>(price, qty);
}

void MarketSnapshot::update_ask(double price, int qty) {
    if (qty <= 0) {
        asks_.erase(price);
        return;
    }
    asks_[price] = std::make_unique<PriceLevel>(price, qty);
}

const PriceLevel* MarketSnapshot::get_best_bid() const {
    if (bids_.empty()) {
        return nullptr;
    }
    return bids_.begin()->second.get();
}

const PriceLevel* MarketSnapshot::get_best_ask() const {
    if (asks_.empty()) {
        return nullptr;
    }
    return asks_.begin()->second.get();
}
