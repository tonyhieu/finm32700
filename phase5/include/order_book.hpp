#pragma once

#include <map>
#include <string>

struct Order {
  std::string id;
  double price;
  int quantity;
  bool isBuy;
};

class OrderBook {
public:
  bool addOrder(const std::string &id, double price, int quantity, bool isBuy) {
    if (quantity <= 0 || orderLookup_.count(id) > 0) {
      return false;
    }

    Order order{id, price, quantity, isBuy};
    if (isBuy) {
      bids_[price][id] = order;
    } else {
      asks_[price][id] = order;
    }
    orderLookup_[id] = order;
    return true;
  }

  bool modifyOrder(const std::string &id, double newPrice, int newQuantity) {
    auto it = orderLookup_.find(id);
    if (it == orderLookup_.end() || newQuantity <= 0) {
      return false;
    }

    Order oldOrder = it->second;
    eraseFromLevel(oldOrder);

    Order updated{id, newPrice, newQuantity, oldOrder.isBuy};
    if (updated.isBuy) {
      bids_[newPrice][id] = updated;
    } else {
      asks_[newPrice][id] = updated;
    }
    it->second = updated;
    return true;
  }

  bool deleteOrder(const std::string &id) {
    auto it = orderLookup_.find(id);
    if (it == orderLookup_.end()) {
      return false;
    }

    eraseFromLevel(it->second);
    orderLookup_.erase(it);
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
  }

private:
  using BidLevels = std::map<double, std::unordered_map<std::string, Order>,
                             std::greater<double>>;
  using AskLevels = std::map<double, std::unordered_map<std::string, Order>>;

  void eraseFromLevel(const Order &order) {
    if (order.isBuy) {
      auto levelIt = bids_.find(order.price);
      if (levelIt == bids_.end()) {
        return;
      }
      levelIt->second.erase(order.id);
      if (levelIt->second.empty()) {
        bids_.erase(levelIt);
      }
    } else {
      auto levelIt = asks_.find(order.price);
      if (levelIt == asks_.end()) {
        return;
      }
      levelIt->second.erase(order.id);
      if (levelIt->second.empty()) {
        asks_.erase(levelIt);
      }
    }
  }

  BidLevels bids_;
  AskLevels asks_;
  std::unordered_map<std::string, Order> orderLookup_;
};
