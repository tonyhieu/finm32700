#include "matrix_operations.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

// Aligned memory allocation
void *aligned_alloc_custom(size_t alignment, size_t size) {
#ifdef _WIN32
  return _aligned_malloc(size, alignment);
#else
  void *ptr = nullptr;
  posix_memalign(&ptr, alignment, size);
  return ptr;
#endif
}

void aligned_free_custom(void *ptr) {
#ifdef _WIN32
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}

struct BenchmarkResult {
  double mean_ms;
  double stddev_ms;
  double min_ms;
  double max_ms;
};

BenchmarkResult benchmark_function(std::function<void()> func,
                                   int iterations = 10) {
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
    if (t < min_val)
      min_val = t;
    if (t > max_val)
      max_val = t;
  }

  double mean = sum / iterations;
  double variance = (sum_sq / iterations) - (mean * mean);
  double stddev = std::sqrt(variance);

  return {mean, stddev, min_val, max_val};
}

// Test 1: Matrix-Vector multiplication cache locality (row vs col major)
void benchmark_mv_cache_locality() {
  std::cout << "\n=== BENCHMARK 1: Matrix-Vector Cache Locality ==="
            << std::endl;
  std::cout << "Comparing row-major vs column-major access patterns\n"
            << std::endl;

  int sizes[] = {100, 500, 1000, 2000};

  std::cout << std::setw(10) << "Size" << std::setw(15) << "Row-Major (ms)"
            << std::setw(15) << "Col-Major (ms)" << std::setw(15) << "Speedup"
            << std::endl;
  std::cout << std::string(55, '-') << std::endl;

  for (int size : sizes) {
    int rows = size;
    int cols = size;

    // Allocate matrices
    double *matrix_row = new double[rows * cols];
    double *matrix_col = new double[rows * cols];
    double *vector = new double[cols];
    double *result = new double[rows];

    // Initialize with random data
    for (int i = 0; i < rows * cols; ++i) {
      matrix_row[i] = (double)rand() / RAND_MAX;
    }

    // Convert to column-major
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        matrix_col[c * rows + r] = matrix_row[r * cols + c];
      }
    }

    for (int i = 0; i < cols; ++i) {
      vector[i] = (double)rand() / RAND_MAX;
    }

    // Benchmark row-major
    auto result_row = benchmark_function([&]() {
      multiply_mv_row_major(matrix_row, rows, cols, vector, result);
    });

    // Benchmark column-major
    auto result_col = benchmark_function([&]() {
      multiply_mv_col_major(matrix_col, rows, cols, vector, result);
    });

    double speedup = result_col.mean_ms / result_row.mean_ms;

    std::cout << std::setw(10) << size << std::setw(15) << std::fixed
              << std::setprecision(4) << result_row.mean_ms << std::setw(15)
              << result_col.mean_ms << std::setw(15) << std::setprecision(2)
              << speedup << "x" << std::endl;

    delete[] matrix_row;
    delete[] matrix_col;
    delete[] vector;
    delete[] result;
  }
}

// Test 2: Matrix-Matrix multiplication cache locality (naive vs transposed)
void benchmark_mm_cache_locality() {
  std::cout << "\n=== BENCHMARK 2: Matrix-Matrix Cache Locality ==="
            << std::endl;
  std::cout << "Comparing naive vs transposed B access patterns\n" << std::endl;

  int sizes[] = {64, 128, 256, 512};

  std::cout << std::setw(10) << "Size" << std::setw(15) << "Naive (ms)"
            << std::setw(18) << "Transposed B (ms)" << std::setw(15)
            << "Speedup" << std::endl;
  std::cout << std::string(58, '-') << std::endl;

  for (int size : sizes) {
    double *matrixA = new double[size * size];
    double *matrixB = new double[size * size];
    double *matrixB_T = new double[size * size];
    double *result = new double[size * size];

    // Initialize with random data
    for (int i = 0; i < size * size; ++i) {
      matrixA[i] = (double)rand() / RAND_MAX;
      matrixB[i] = (double)rand() / RAND_MAX;
    }

    // Transpose B
    for (int r = 0; r < size; ++r) {
      for (int c = 0; c < size; ++c) {
        matrixB_T[c * size + r] = matrixB[r * size + c];
      }
    }

    // Benchmark naive
    auto result_naive = benchmark_function(
        [&]() {
          multiply_mm_naive(matrixA, size, size, matrixB, size, size, result);
        },
        5);

    // Benchmark transposed
    auto result_transposed = benchmark_function(
        [&]() {
          multiply_mm_transposed_b(matrixA, size, size, matrixB_T, size, size,
                                   result);
        },
        5);

    double speedup = result_naive.mean_ms / result_transposed.mean_ms;

    std::cout << std::setw(10) << size << std::setw(15) << std::fixed
              << std::setprecision(4) << result_naive.mean_ms << std::setw(18)
              << result_transposed.mean_ms << std::setw(15)
              << std::setprecision(2) << speedup << "x" << std::endl;

    delete[] matrixA;
    delete[] matrixB;
    delete[] matrixB_T;
    delete[] result;
  }
}

// Test 3: Memory alignment impact
void benchmark_alignment() {
  std::cout << "\n=== BENCHMARK 3: Memory Alignment Impact ===" << std::endl;
  std::cout << "Comparing unaligned vs 64-byte aligned memory\n" << std::endl;

  int sizes[] = {256, 512, 1024};

  std::cout << std::setw(10) << "Size" << std::setw(18) << "Unaligned (ms)"
            << std::setw(18) << "64-byte Aligned (ms)" << std::setw(15)
            << "Speedup" << std::endl;
  std::cout << std::string(61, '-') << std::endl;

  for (int size : sizes) {
    // Unaligned allocation
    double *A_unaligned = new double[size * size];
    double *B_unaligned = new double[size * size];
    double *C_unaligned = new double[size * size];

    // Aligned allocation (64-byte boundary)
    double *A_aligned =
        (double *)aligned_alloc_custom(64, size * size * sizeof(double));
    double *B_aligned =
        (double *)aligned_alloc_custom(64, size * size * sizeof(double));
    double *C_aligned =
        (double *)aligned_alloc_custom(64, size * size * sizeof(double));

    // Initialize both
    for (int i = 0; i < size * size; ++i) {
      A_unaligned[i] = A_aligned[i] = (double)rand() / RAND_MAX;
      B_unaligned[i] = B_aligned[i] = (double)rand() / RAND_MAX;
    }

    // Benchmark unaligned
    auto result_unaligned = benchmark_function(
        [&]() {
          multiply_mm_naive(A_unaligned, size, size, B_unaligned, size, size,
                            C_unaligned);
        },
        5);

    // Benchmark aligned
    auto result_aligned = benchmark_function(
        [&]() {
          multiply_mm_naive(A_aligned, size, size, B_aligned, size, size,
                            C_aligned);
        },
        5);

    double speedup = result_unaligned.mean_ms / result_aligned.mean_ms;

    std::cout << std::setw(10) << size << std::setw(18) << std::fixed
              << std::setprecision(4) << result_unaligned.mean_ms
              << std::setw(18) << result_aligned.mean_ms << std::setw(15)
              << std::setprecision(2) << speedup << "x" << std::endl;

    delete[] A_unaligned;
    delete[] B_unaligned;
    delete[] C_unaligned;
    aligned_free_custom(A_aligned);
    aligned_free_custom(B_aligned);
    aligned_free_custom(C_aligned);
  }
}

int main() {
  std::cout << "==========================================" << std::endl;
  std::cout << "  Cache Locality & Alignment Benchmarks  " << std::endl;
  std::cout << "==========================================" << std::endl;

  benchmark_mv_cache_locality();
  benchmark_mm_cache_locality();
  benchmark_alignment();

  std::cout << "\n==========================================" << std::endl;
  std::cout << "Benchmarking completed!" << std::endl;
  std::cout << "==========================================" << std::endl;

  return 0;
}
