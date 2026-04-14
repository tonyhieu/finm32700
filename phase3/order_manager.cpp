#include "order_manager.h"

#include <algorithm>
#include <iostream>

int OrderManager::place_order(Side side, double price, int qty) {
    const int id = next_id_++;
    auto order = std::make_unique<MyOrder>();
    order->id = id;
    order->side = side;
    order->price = price;
    order->quantity = qty;
    orders_[id] = std::move(order);

    std::cout << "[Strategy] Placing " << side_to_string(side) << " order at "
              << price << " x " << qty << " (ID = " << id << ")\n";

    return id;
}

void OrderManager::cancel(int id) {
    const auto it = orders_.find(id);
    if (it == orders_.end()) {
        std::cout << "[Order] Cancel ignored: order " << id << " not found\n";
        return;
    }

    it->second->status = OrderStatus::Cancelled;
    std::cout << "[Order] Order " << id << " cancelled and removed\n";
    orders_.erase(it);
}

void OrderManager::handle_fill(int id, int filled_qty) {
    const auto it = orders_.find(id);
    if (it == orders_.end()) {
        std::cout << "[Execution] Fill ignored: order " << id << " not found\n";
        return;
    }
    if (filled_qty <= 0) {
        std::cout << "[Execution] Fill ignored: non-positive fill for order " << id
                  << "\n";
        return;
    }

    MyOrder& order = *(it->second);
    const int remaining = order.quantity - order.filled;
    const int applied_fill = std::min(remaining, filled_qty);
    order.filled += applied_fill;

    std::cout << "[Execution] Order " << id << " filled: " << applied_fill << "\n";

    if (order.filled < order.quantity) {
        order.status = OrderStatus::PartiallyFilled;
        std::cout << "[Order] Order " << id << " partially filled: " << order.filled
                  << " / " << order.quantity << "\n";
        return;
    }

    order.status = OrderStatus::Filled;
    std::cout << "[Order] Order " << id << " completed (" << order.filled << " / "
              << order.quantity << ") and removed\n";
    orders_.erase(it);
}

void OrderManager::print_active_orders() const {
    std::cout << "[Order] Active orders: " << orders_.size() << "\n";
    for (const auto& [id, order] : orders_) {
        std::cout << "  ID " << id << " "
                  << side_to_string(order->side) << " @ " << order->price << " qty "
                  << order->quantity << " filled " << order->filled << "\n";
    }
}

std::size_t OrderManager::active_order_count() const { return orders_.size(); }

std::string OrderManager::side_to_string(Side side) {
    return side == Side::Buy ? "BUY" : "SELL";
}
