#pragma once

#include "Order.hpp"

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

template <typename T>
class MemoryPool {
public:
    explicit MemoryPool(std::size_t reserve_count = 0) {
        storage_.reserve(reserve_count);
    }

    template <typename... Args>
    T* create(Args&&... args) {
        storage_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return storage_.back().get();
    }

private:
    std::vector<std::unique_ptr<T>> storage_;
};

template <typename PriceType, typename OrderIdType>
class OrderBook {
public:
    using OrderType = Order<PriceType, OrderIdType>;

    explicit OrderBook(std::size_t reserve_count = 0)
        : pool_(reserve_count) {}

    OrderType* addOrder(OrderType incoming) {
        OrderType* order_ptr = pool_.create(
            incoming.id,
            std::move(incoming.symbol),
            incoming.price,
            incoming.quantity,
            incoming.is_buy);
        id_index_[order_ptr->id] = order_ptr;
        if (order_ptr->is_buy) {
            bids_.emplace(order_ptr->price, order_ptr);
        } else {
            asks_.emplace(order_ptr->price, order_ptr);
        }
        return order_ptr;
    }

    bool cancelOrder(OrderIdType id) {
        const auto id_it = id_index_.find(id);
        if (id_it == id_index_.end()) {
            return false;
        }
        OrderType* target = id_it->second;
        if (target->is_buy) {
            auto [begin_it, end_it] = bids_.equal_range(target->price);
            for (auto it = begin_it; it != end_it; ++it) {
                if (it->second->id == id) {
                    bids_.erase(it);
                    break;
                }
            }
        } else {
            auto [begin_it, end_it] = asks_.equal_range(target->price);
            for (auto it = begin_it; it != end_it; ++it) {
                if (it->second->id == id) {
                    asks_.erase(it);
                    break;
                }
            }
        }
        id_index_.erase(id_it);
        return true;
    }

    bool hasOppositeLiquidity(bool incoming_is_buy) const {
        return incoming_is_buy ? !asks_.empty() : !bids_.empty();
    }

    OrderType* bestOpposite(bool incoming_is_buy) {
        if (!hasOppositeLiquidity(incoming_is_buy)) {
            return nullptr;
        }
        if (incoming_is_buy) {
            return asks_.begin()->second;
        }
        return bids_.begin()->second;
    }

    bool crossesSpread(const OrderType& incoming, const OrderType& opposite) const {
        if (incoming.is_buy) {
            return incoming.price >= opposite.price;
        }
        return incoming.price <= opposite.price;
    }

    void reduceOrRemove(OrderIdType id, int quantity_traded) {
        const auto id_it = id_index_.find(id);
        if (id_it == id_index_.end()) {
            return;
        }
        OrderType* order = id_it->second;
        order->quantity -= quantity_traded;
        if (order->quantity > 0) {
            return;
        }
        cancelOrder(id);
    }

    std::optional<PriceType> bestBid() const {
        if (bids_.empty()) {
            return std::nullopt;
        }
        return bids_.begin()->first;
    }

    std::optional<PriceType> bestAsk() const {
        if (asks_.empty()) {
            return std::nullopt;
        }
        return asks_.begin()->first;
    }

private:
    std::multimap<PriceType, OrderType*, std::greater<PriceType>> bids_;
    std::multimap<PriceType, OrderType*> asks_;
    std::unordered_map<OrderIdType, OrderType*> id_index_;
    MemoryPool<OrderType> pool_;
};
