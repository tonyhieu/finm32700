#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "hw5/order_book.hpp"
#include "hw5/order_priority_queue.hpp"
#include "hw5/robin_hood_hash_table.hpp"

namespace hw5 {

struct BenchmarkStats {
    std::string label;
    double mean_ms;
    double median_ms;
    std::size_t operations;
};

BenchmarkStats benchmark_hash_table_against_std(std::size_t element_count, std::size_t iterations);
BenchmarkStats benchmark_priority_queue_against_std(std::size_t element_count, std::size_t iterations);
BenchmarkStats benchmark_order_book_workload(const std::vector<BookOrder>& workload, std::size_t iterations);

}  // namespace hw5
