#pragma once

#include "order_book.hpp"

#include <map>
#include <string>

class OptimizedOrderBook {
public:
  explicit OptimizedOrderBook(std::size_t expectedOrders = 200000) {
    reserve(expectedOrders);
  }

  void reserve(std::size_t expectedOrders) {
    orderPool_.reserve(expectedOrders);
    active_.reserve(expectedOrders);
    freeIndices_.reserve(expectedOrders);
    orderLookup_.reserve(expectedOrders * 2);
    orderLookup_.max_load_factor(0.70f);
  }

  bool addOrder(const std::string &id, double price, int quantity, bool isBuy) {
    if (quantity <= 0 || orderLookup_.count(id) > 0) {
      return false;
    }

    Order order{id, price, quantity, isBuy};
    const std::size_t index = acquireSlot(order);

    if (isBuy) {
      bids_[price][id] = index;
    } else {
      asks_[price][id] = index;
    }
    orderLookup_[id] = index;
    return true;
  }

  bool modifyOrder(const std::string &id, double newPrice, int newQuantity) {
    auto it = orderLookup_.find(id);
    if (it == orderLookup_.end() || newQuantity <= 0) {
      return false;
    }

    const std::size_t index = it->second;
    Order &order = orderPool_[index];
    eraseFromLevel(order.id, order.price, order.isBuy);

    order.price = newPrice;
    order.quantity = newQuantity;
    if (order.isBuy) {
      bids_[newPrice][id] = index;
    } else {
      asks_[newPrice][id] = index;
    }
    return true;
  }

  bool deleteOrder(const std::string &id) {
    auto it = orderLookup_.find(id);
    if (it == orderLookup_.end()) {
      return false;
    }

    const std::size_t index = it->second;
    const Order &order = orderPool_[index];
    eraseFromLevel(order.id, order.price, order.isBuy);
    orderLookup_.erase(it);
    releaseSlot(index);
    return true;
  }

  bool hasOrder(const std::string &id) const {
    return orderLookup_.count(id) > 0;
  }

  std::size_t orderCount() const { return orderLookup_.size(); }

  std::size_t bidLevels() const { return bids_.size(); }

  std::size_t askLevels() const { return asks_.size(); }

  std::pair<bool, double> bestBid() const {
    if (bids_.empty()) {
      return {false, 0.0};
    }
    return {true, bids_.begin()->first};
  }

  std::pair<bool, double> bestAsk() const {
    if (asks_.empty()) {
      return {false, 0.0};
    }
    return {true, asks_.begin()->first};
  }

  void clear() {
    bids_.clear();
    asks_.clear();
    orderLookup_.clear();
    freeIndices_.clear();

    for (std::size_t i = 0; i < active_.size(); ++i) {
      if (active_[i]) {
        active_[i] = 0;
        freeIndices_.push_back(i);
      }
    }
  }

private:
  using BidLevels =
      std::map<double, std::unordered_map<std::string, std::size_t>,
               std::greater<double>>;
  using AskLevels =
      std::map<double, std::unordered_map<std::string, std::size_t>>;

  std::size_t acquireSlot(const Order &order) {
    if (!freeIndices_.empty()) {
      const std::size_t index = freeIndices_.back();
      freeIndices_.pop_back();
      orderPool_[index] = order;
      active_[index] = 1;
      return index;
    }

    orderPool_.push_back(order);
    active_.push_back(1);
    return orderPool_.size() - 1;
  }

  void releaseSlot(std::size_t index) {
    active_[index] = 0;
    freeIndices_.push_back(index);
  }

  void eraseFromLevel(const std::string &id, double price, bool isBuy) {
    if (isBuy) {
      auto levelIt = bids_.find(price);
      if (levelIt == bids_.end()) {
        return;
      }
      levelIt->second.erase(id);
      if (levelIt->second.empty()) {
        bids_.erase(levelIt);
      }
    } else {
      auto levelIt = asks_.find(price);
      if (levelIt == asks_.end()) {
        return;
      }
      levelIt->second.erase(id);
      if (levelIt->second.empty()) {
        asks_.erase(levelIt);
      }
    }
  }

  BidLevels bids_;
  AskLevels asks_;
  std::vector<Order> orderPool_;
  std::vector<char> active_;
  std::vector<std::size_t> freeIndices_;
  std::unordered_map<std::string, std::size_t> orderLookup_;
};
