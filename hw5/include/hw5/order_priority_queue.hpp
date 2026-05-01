#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace hw5 {

enum class HeapType {
    Max,
    Min
};

struct Order {
    int id;
    double price;
    int quantity;
    std::int64_t timestamp;
    char side;

    bool operator<(const Order& other) const { return price < other.price; }
};

class OrderPriorityQueue {
public:
    explicit OrderPriorityQueue(HeapType heap_type = HeapType::Max);

    void push(const Order& order);
    Order pop();
    const Order& top() const;

    bool empty() const noexcept;
    std::size_t size() const noexcept;

private:
    std::vector<Order> heap_;
    HeapType heap_type_;

    bool compare(const Order& lhs, const Order& rhs) const;
    void sift_up(std::size_t index);
    void sift_down(std::size_t index);
};

}  // namespace hw5
