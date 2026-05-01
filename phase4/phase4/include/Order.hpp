#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

enum class OrderStatus {
    New,
    Cancelled,
    Partial,
    Filled
};

template <typename PriceType, typename OrderIdType>
struct Order {
    static_assert(std::is_integral<OrderIdType>::value, "Order ID must be an integer");

    OrderIdType id;
    std::string symbol;
    PriceType price;
    int quantity;
    bool is_buy;

    Order(OrderIdType id_, std::string sym, PriceType pr, int qty, bool buy)
        : id(id_), symbol(std::move(sym)), price(pr), quantity(qty), is_buy(buy) {}
};

template <typename PriceType, typename OrderIdType>
using OrderPtr = std::unique_ptr<Order<PriceType, OrderIdType>>;
