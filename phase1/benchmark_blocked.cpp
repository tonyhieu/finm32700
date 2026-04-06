#include "matrix_operations.h"
#include "matrix_operations_optimized.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstdlib>

struct BenchmarkResult {
  double mean_ms;
  double stddev_ms;
  double min_ms;
  double max_ms;
};

BenchmarkResult benchmark_function(std::function<void()> func, int iterations = 10) {
  std::vector<double> times;

  for (int i = 0; i < iterations; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    times.push_back(ms);
  }

  // Calculate statistics
  double sum = 0, sum_sq = 0;
  double min_val = times[0], max_val = times[0];

  for (double t : times) {
    sum += t;
    sum_sq += t * t;
    if (t < min_val) min_val = t;
    if (t > max_val) max_val = t;
  }

  double mean = sum / iterations;
  double variance = (sum_sq / iterations) - (mean * mean);
  double stddev = std::sqrt(variance);

  return {mean, stddev, min_val, max_val};
}

bool verify_results(const double *result1, const double *result2, int size, double tolerance = 1e-9) {
  for (int i = 0; i < size; ++i) {
    if (std::abs(result1[i] - result2[i]) > tolerance) {
      std::cerr << "Mismatch at index " << i << ": " 
                << result1[i] << " vs " << result2[i] << std::endl;
      return false;
    }
  }
  return true;
}

void benchmark_naive_vs_blocked() {
  std::cout << "\n=== Matrix Multiplication: Naive vs Blocked/Tiled ===" << std::endl;
  std::cout << "Comparing cache-oblivious naive vs cache-blocked implementation\n" << std::endl;
  
  int sizes[] = {128, 256, 512, 1024};
  
  std::cout << std::setw(10) << "Size" 
            << std::setw(18) << "Naive (ms)"
            << std::setw(18) << "Blocked (ms)"
            << std::setw(15) << "Speedup"
            << std::setw(18) << "Verified"
            << std::endl;
  std::cout << std::string(79, '-') << std::endl;

  for (int size : sizes) {
    double *A = new double[size * size];
    double *B = new double[size * size];
    double *C_naive = new double[size * size];
    double *C_blocked = new double[size * size];

    // Initialize with random data
    for (int i = 0; i < size * size; ++i) {
      A[i] = (double)rand() / RAND_MAX;
      B[i] = (double)rand() / RAND_MAX;
    }

    // Benchmark naive implementation
    auto result_naive = benchmark_function([&]() {
      multiply_mm_naive(A, size, size, B, size, size, C_naive);
    }, 5);

    // Benchmark blocked implementation
    auto result_blocked = benchmark_function([&]() {
      multiply_mm_blocked(A, size, size, B, size, size, C_blocked);
    }, 5);

    // Verify correctness
    bool correct = verify_results(C_naive, C_blocked, size * size);
    
    double speedup = result_naive.mean_ms / result_blocked.mean_ms;

    std::cout << std::setw(10) << size 
              << std::setw(18) << std::fixed << std::setprecision(4) 
              << result_naive.mean_ms
              << std::setw(18) << result_blocked.mean_ms
              << std::setw(15) << std::setprecision(2) << speedup << "x"
              << std::setw(18) << (correct ? "✓" : "✗")
              << std::endl;

    delete[] A;
    delete[] B;
    delete[] C_naive;
    delete[] C_blocked;
  }
}

void benchmark_all_variants() {
  std::cout << "\n=== Comparison: All Matrix Multiplication Variants ===" << std::endl;
  std::cout << "Testing: Naive, Transposed B, and Blocked implementations\n" << std::endl;
  
  int sizes[] = {256, 512};
  
  std::cout << std::setw(10) << "Size" 
            << std::setw(15) << "Naive (ms)"
            << std::setw(18) << "Transposed B (ms)"
            << std::setw(15) << "Blocked (ms)"
            << std::endl;
  std::cout << std::string(58, '-') << std::endl;

  for (int size : sizes) {
    double *A = new double[size * size];
    double *B = new double[size * size];
    double *B_T = new double[size * size];
    double *C = new double[size * size];

    // Initialize
    for (int i = 0; i < size * size; ++i) {
      A[i] = (double)rand() / RAND_MAX;
      B[i] = (double)rand() / RAND_MAX;
    }

    // Transpose B
    for (int i = 0; i < size; ++i) {
      for (int j = 0; j < size; ++j) {
        B_T[j * size + i] = B[i * size + j];
      }
    }

    // Benchmark all three
    auto result_naive = benchmark_function([&]() {
      multiply_mm_naive(A, size, size, B, size, size, C);
    }, 5);

    auto result_transposed = benchmark_function([&]() {
      multiply_mm_transposed_b(A, size, size, B_T, size, size, C);
    }, 5);

    auto result_blocked = benchmark_function([&]() {
      multiply_mm_blocked(A, size, size, B, size, size, C);
    }, 5);

    std::cout << std::setw(10) << size 
              << std::setw(15) << std::fixed << std::setprecision(4) 
              << result_naive.mean_ms
              << std::setw(18) << result_transposed.mean_ms
              << std::setw(15) << result_blocked.mean_ms
              << std::endl;

    delete[] A;
    delete[] B;
    delete[] B_T;
    delete[] C;
  }
}

int main() {
  std::cout << "==========================================" << std::endl;
  std::cout << "   Cache Blocking Optimization Test     " << std::endl;
  std::cout << "==========================================" << std::endl;

  std::cout << "\nThis benchmark compares the naive matrix multiplication" << std::endl;
  std::cout << "implementation against a cache-blocked (tiled) version." << std::endl;
  std::cout << "\nCache blocking improves performance by:" << std::endl;
  std::cout << "  • Dividing matrices into tiles that fit in cache" << std::endl;
  std::cout << "  • Maximizing data reuse while in cache" << std::endl;
  std::cout << "  • Reducing cache misses from O(N³) to O(N³/B)" << std::endl;
  std::cout << "==========================================" << std::endl;

  benchmark_naive_vs_blocked();
  benchmark_all_variants();

  std::cout << "\n==========================================" << std::endl;
  std::cout << "Benchmarking completed!" << std::endl;
  std::cout << "==========================================" << std::endl;

  return 0;
}
