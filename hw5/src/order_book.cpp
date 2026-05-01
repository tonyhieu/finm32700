#include "hw5/order_book.hpp"

#include <algorithm>

bool hw5::OrderBook::add_order(const BookOrder &order) {
  if (order.quantity <= 0 || orders_by_id_.contains(order.id)) {
    return false;
  }

  orders_by_id_[order.id] = order;
  if (order.side == Side::Buy) {
    bids_by_price_[order.price].push_back(order.id);
  } else {
    asks_by_price_[order.price].push_back(order.id);
  }
  return true;
}

bool hw5::OrderBook::modify_order(int order_id, int new_quantity) {
  auto it = orders_by_id_.find(order_id);
  if (it == orders_by_id_.end() || new_quantity <= 0) {
    return false;
  }

  it->second.quantity = new_quantity;
  return true;
}

bool hw5::OrderBook::delete_order(int order_id) {
  auto it = orders_by_id_.find(order_id);
  if (it == orders_by_id_.end()) {
    return false;
  }

  remove_order_from_price_level(it->second);
  orders_by_id_.erase(it);
  return true;
}

std::vector<hw5::BookOrder> hw5::OrderBook::query_price_level(double price,
                                                              Side side) const {
  std::vector<BookOrder> out;
  if (side == Side::Buy) {
    auto level_it = bids_by_price_.find(price);
    if (level_it == bids_by_price_.end()) {
      return out;
    }

    out.reserve(level_it->second.size());
    for (int order_id : level_it->second) {
      auto order_it = orders_by_id_.find(order_id);
      if (order_it != orders_by_id_.end()) {
        out.push_back(order_it->second);
      }
    }
    return out;
  }

  auto level_it = asks_by_price_.find(price);
  if (level_it == asks_by_price_.end()) {
    return out;
  }

  out.reserve(level_it->second.size());
  for (int order_id : level_it->second) {
    auto order_it = orders_by_id_.find(order_id);
    if (order_it != orders_by_id_.end()) {
      out.push_back(order_it->second);
    }
  }
  return out;
}

std::optional<double> hw5::OrderBook::best_bid() const {
  if (bids_by_price_.empty()) {
    return std::nullopt;
  }
  return bids_by_price_.begin()->first;
}

std::optional<double> hw5::OrderBook::best_offer() const {
  if (asks_by_price_.empty()) {
    return std::nullopt;
  }
  return asks_by_price_.begin()->first;
}

std::optional<hw5::BookOrder> hw5::OrderBook::get_order(int order_id) const {
  auto it = orders_by_id_.find(order_id);
  if (it == orders_by_id_.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::size_t hw5::OrderBook::order_count() const noexcept {
  return orders_by_id_.size();
}

void hw5::OrderBook::remove_order_from_price_level(const BookOrder &order) {
  if (order.side == Side::Buy) {
    auto level_it = bids_by_price_.find(order.price);
    if (level_it == bids_by_price_.end()) {
      return;
    }
    auto &ids = level_it->second;
    ids.erase(std::remove(ids.begin(), ids.end(), order.id), ids.end());
    if (ids.empty()) {
      bids_by_price_.erase(level_it);
    }
  } else {
    auto level_it = asks_by_price_.find(order.price);
    if (level_it == asks_by_price_.end()) {
      return;
    }
    auto &ids = level_it->second;
    ids.erase(std::remove(ids.begin(), ids.end(), order.id), ids.end());
    if (ids.empty()) {
      asks_by_price_.erase(level_it);
    }
  }
}
