#pragma once

#include "Order.hpp"

#include <type_traits>
#include <unordered_map>

template <typename OrderIdType>
class OrderManager {
public:
    static_assert(std::is_integral<OrderIdType>::value, "Order ID must be an integer");

    void registerNew(OrderIdType id) {
        states_[id] = OrderStatus::New;
    }

    bool cancel(OrderIdType id) {
        auto it = states_.find(id);
        if (it == states_.end()) {
            return false;
        }
        it->second = OrderStatus::Cancelled;
        return true;
    }

    bool markPartial(OrderIdType id) {
        auto it = states_.find(id);
        if (it == states_.end()) {
            return false;
        }
        it->second = OrderStatus::Partial;
        return true;
    }

    bool markFilled(OrderIdType id) {
        auto it = states_.find(id);
        if (it == states_.end()) {
            return false;
        }
        it->second = OrderStatus::Filled;
        return true;
    }

    OrderStatus getState(OrderIdType id) const {
        auto it = states_.find(id);
        if (it == states_.end()) {
            return OrderStatus::Cancelled;
        }
        return it->second;
    }

private:
    std::unordered_map<OrderIdType, OrderStatus> states_;
};
