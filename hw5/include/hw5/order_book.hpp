#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace hw5 {

enum class Side {
    Buy,
    Sell
};

struct BookOrder {
    int id;
    double price;
    int quantity;
    Side side;
};

class OrderBook {
public:
    bool add_order(const BookOrder& order);
    bool modify_order(int order_id, int new_quantity);
    bool delete_order(int order_id);

    std::vector<BookOrder> query_price_level(double price, Side side) const;
    std::optional<double> best_bid() const;
    std::optional<double> best_offer() const;
    std::optional<BookOrder> get_order(int order_id) const;

    std::size_t order_count() const noexcept;

private:
    std::unordered_map<int, BookOrder> orders_by_id_;
    std::map<double, std::vector<int>, std::greater<double>> bids_by_price_;
    std::map<double, std::vector<int>, std::less<double>> asks_by_price_;

    void remove_order_from_price_level(const BookOrder& order);
};

}  // namespace hw5
