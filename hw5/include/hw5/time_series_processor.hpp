#pragma once

#include <cstddef>
#include <vector>

namespace hw5 {

std::vector<double> moving_average_scalar(const std::vector<double>& prices, std::size_t window);
std::vector<double> moving_average_simd(const std::vector<double>& prices, std::size_t window);

struct TimeSeriesBenchmarkResult {
    double scalar_ms;
    double simd_ms;
};

TimeSeriesBenchmarkResult benchmark_moving_average(
    const std::vector<double>& prices,
    std::size_t window,
    std::size_t iterations);

}  // namespace hw5
