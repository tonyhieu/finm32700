#include "hw5/time_series_processor.hpp"

#include <chrono>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) ||               \
    defined(_M_IX86)
#include <immintrin.h>
#define HW5_X86_SIMD 1
#endif

std::vector<double>
hw5::moving_average_scalar(const std::vector<double> &prices,
                           std::size_t window) {
  if (window == 0 || prices.size() < window) {
    return {};
  }

  std::vector<double> prefix(prices.size() + 1, 0.0);
  for (std::size_t i = 0; i < prices.size(); ++i) {
    prefix[i + 1] = prefix[i] + prices[i];
  }

  const std::size_t out_size = prices.size() - window + 1;
  std::vector<double> out(out_size, 0.0);
  const double inv_window = 1.0 / static_cast<double>(window);
  for (std::size_t i = 0; i < out_size; ++i) {
    out[i] = (prefix[i + window] - prefix[i]) * inv_window;
  }
  return out;
}

std::vector<double> hw5::moving_average_simd(const std::vector<double> &prices,
                                             std::size_t window) {
  if (window == 0 || prices.size() < window) {
    return {};
  }

  std::vector<double> prefix(prices.size() + 1, 0.0);
  for (std::size_t i = 0; i < prices.size(); ++i) {
    prefix[i + 1] = prefix[i] + prices[i];
  }

  const std::size_t out_size = prices.size() - window + 1;
  std::vector<double> out(out_size, 0.0);
  const double inv_window = 1.0 / static_cast<double>(window);
  std::size_t i = 0;

#if defined(HW5_X86_SIMD) && defined(__AVX2__)
  const __m256d inv_vec = _mm256_set1_pd(inv_window);
  for (; i + 4 <= out_size; i += 4) {
    __m256d high = _mm256_loadu_pd(&prefix[i + window]);
    __m256d low = _mm256_loadu_pd(&prefix[i]);
    __m256d sum = _mm256_sub_pd(high, low);
    __m256d avg = _mm256_mul_pd(sum, inv_vec);
    _mm256_storeu_pd(&out[i], avg);
  }
#elif defined(HW5_X86_SIMD) && defined(__SSE2__)
  const __m128d inv_vec = _mm_set1_pd(inv_window);
  for (; i + 2 <= out_size; i += 2) {
    __m128d high = _mm_loadu_pd(&prefix[i + window]);
    __m128d low = _mm_loadu_pd(&prefix[i]);
    __m128d sum = _mm_sub_pd(high, low);
    __m128d avg = _mm_mul_pd(sum, inv_vec);
    _mm_storeu_pd(&out[i], avg);
  }
#endif

  for (; i < out_size; ++i) {
    out[i] = (prefix[i + window] - prefix[i]) * inv_window;
  }
  return out;
}

hw5::TimeSeriesBenchmarkResult
hw5::benchmark_moving_average(const std::vector<double> &prices,
                              std::size_t window, std::size_t iterations) {
  if (iterations == 0) {
    return {0.0, 0.0};
  }

  double scalar_total_ms = 0.0;
  double simd_total_ms = 0.0;
  volatile double sink = 0.0;

  for (std::size_t i = 0; i < iterations; ++i) {
    const auto scalar_start = std::chrono::steady_clock::now();
    const auto scalar_out = moving_average_scalar(prices, window);
    const auto scalar_end = std::chrono::steady_clock::now();
    scalar_total_ms +=
        std::chrono::duration<double, std::milli>(scalar_end - scalar_start)
            .count();

    const auto simd_start = std::chrono::steady_clock::now();
    const auto simd_out = moving_average_simd(prices, window);
    const auto simd_end = std::chrono::steady_clock::now();
    simd_total_ms +=
        std::chrono::duration<double, std::milli>(simd_end - simd_start)
            .count();

    if (!scalar_out.empty()) {
      sink += scalar_out[0];
    }
    if (!simd_out.empty()) {
      sink += simd_out[0];
    }
  }

  if (sink < 0.0) {
    sink = 0.0;
  }

  return {scalar_total_ms / static_cast<double>(iterations),
          simd_total_ms / static_cast<double>(iterations)};
}
