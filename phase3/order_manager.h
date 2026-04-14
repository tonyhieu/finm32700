#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <string>

enum class Side { Buy, Sell };
enum class OrderStatus { New, Filled, PartiallyFilled, Cancelled };

struct MyOrder {
    int id;
    Side side;
    double price;
    int quantity;
    int filled = 0;
    OrderStatus status = OrderStatus::New;
};

class OrderManager {
public:
    int place_order(Side side, double price, int qty);
    void cancel(int id);
    void handle_fill(int id, int filled_qty);
    void print_active_orders() const;
    std::size_t active_order_count() const;

private:
    static std::string side_to_string(Side side);

    int next_id_ = 1;
    std::map<int, std::unique_ptr<MyOrder>> orders_;
};
