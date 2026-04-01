#include "matrix_operations.h"
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

void benchmark_mv_functions() {
  std::cout << "\n=== Matrix-Vector Multiplication Performance ===" << std::endl;
  
  int sizes[] = {100, 500, 1000, 2000};
  
  std::cout << std::setw(10) << "Size" 
            << std::setw(18) << "Row-Major (ms)"
            << std::setw(18) << "Col-Major (ms)"
            << std::endl;
  std::cout << std::string(46, '-') << std::endl;

  for (int size : sizes) {
    double *matrix = new double[size * size];
    double *vector = new double[size];
    double *result = new double[size];

    // Initialize
    for (int i = 0; i < size * size; ++i) {
      matrix[i] = (double)rand() / RAND_MAX;
    }
    for (int i = 0; i < size; ++i) {
      vector[i] = (double)rand() / RAND_MAX;
    }

    // Benchmark row-major
    auto result_row = benchmark_function([&]() {
      multiply_mv_row_major(matrix, size, size, vector, result);
    }, 10);

    // Benchmark col-major
    auto result_col = benchmark_function([&]() {
      multiply_mv_col_major(matrix, size, size, vector, result);
    }, 10);

    std::cout << std::setw(10) << size 
              << std::setw(18) << std::fixed << std::setprecision(4) 
              << result_row.mean_ms
              << std::setw(18) << result_col.mean_ms
              << std::endl;

    delete[] matrix;
    delete[] vector;
    delete[] result;
  }
}

void benchmark_mm_functions() {
  std::cout << "\n=== Matrix-Matrix Multiplication Performance ===" << std::endl;
  
  int sizes[] = {64, 128, 256, 512};
  
  std::cout << std::setw(10) << "Size" 
            << std::setw(18) << "Naive (ms)"
            << std::setw(18) << "Transposed B (ms)"
            << std::endl;
  std::cout << std::string(46, '-') << std::endl;

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

    // Benchmark naive
    auto result_naive = benchmark_function([&]() {
      multiply_mm_naive(A, size, size, B, size, size, C);
    }, 5);

    // Benchmark transposed
    auto result_trans = benchmark_function([&]() {
      multiply_mm_transposed_b(A, size, size, B_T, size, size, C);
    }, 5);

    std::cout << std::setw(10) << size 
              << std::setw(18) << std::fixed << std::setprecision(4) 
              << result_naive.mean_ms
              << std::setw(18) << result_trans.mean_ms
              << std::endl;

    delete[] A;
    delete[] B;
    delete[] B_T;
    delete[] C;
  }
}

int main() {
  std::cout << "==========================================" << std::endl;
  std::cout << "   Compiler Optimization Impact Test    " << std::endl;
  std::cout << "==========================================" << std::endl;

#ifdef __OPTIMIZE__
  std::cout << "Compiled with optimizations enabled" << std::endl;
#else
  std::cout << "Compiled with optimizations disabled (-O0)" << std::endl;
#endif

  benchmark_mv_functions();
  benchmark_mm_functions();

  std::cout << "\n==========================================" << std::endl;
  std::cout << "Benchmarking completed!" << std::endl;
  std::cout << "==========================================" << std::endl;

  return 0;
}
