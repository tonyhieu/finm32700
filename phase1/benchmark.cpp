#include "matrix_operations.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

using time_clock = std::chrono::high_resolution_clock;

double random_data(double low, double hi) {
  double r = (double)std::rand() / (double)RAND_MAX;
  return low + r * (hi - low);
}

void fill_matrix(double *matrix, int size) {
  for (int i = 0; i < size; ++i) {
    matrix[i] = random_data(-10.0, 10.0);
  }
}

void fill_vector(double *vector, int size) {
  for (int i = 0; i < size; ++i) {
    vector[i] = random_data(-10.0, 10.0);
  }
}

struct BenchmarkResult {
  double avg_time_ms;
  double std_dev_ms;
};

BenchmarkResult calculate_statistics(const std::vector<double> &times) {
  double sum = 0.0;
  for (double t : times) {
    sum += t;
  }
  double avg = sum / times.size();

  double variance = 0.0;
  for (double t : times) {
    variance += (t - avg) * (t - avg);
  }
  variance /= times.size();
  double std_dev = std::sqrt(variance);

  return {avg, std_dev};
}

void benchmark_mv_row_major(int rows, int cols, int num_runs) {
  std::cout << "\n=== Matrix-Vector (Row-Major) Benchmark ===" << std::endl;
  std::cout << "Matrix size: " << rows << "x" << cols << std::endl;
  std::cout << "Number of runs: " << num_runs << std::endl;

  double *matrix = new double[rows * cols];
  double *vector = new double[cols];
  double *result = new double[rows];

  fill_matrix(matrix, rows * cols);
  fill_vector(vector, cols);

  std::vector<double> times;
  for (int i = 0; i < num_runs; ++i) {
    time_clock::time_point t1 = time_clock::now();
    multiply_mv_row_major(matrix, rows, cols, vector, result);
    time_clock::time_point t2 = time_clock::now();

    double elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() /
        1000.0;
    times.push_back(elapsed);
  }

  BenchmarkResult stats = calculate_statistics(times);
  std::cout << "Average time: " << stats.avg_time_ms << " ms" << std::endl;
  std::cout << "Std deviation: " << stats.std_dev_ms << " ms" << std::endl;

  delete[] matrix;
  delete[] vector;
  delete[] result;
}

void benchmark_mv_col_major(int rows, int cols, int num_runs) {
  std::cout << "\n=== Matrix-Vector (Column-Major) Benchmark ===" << std::endl;
  std::cout << "Matrix size: " << rows << "x" << cols << std::endl;
  std::cout << "Number of runs: " << num_runs << std::endl;

  double *matrix = new double[rows * cols];
  double *vector = new double[cols];
  double *result = new double[rows];

  fill_matrix(matrix, rows * cols);
  fill_vector(vector, cols);

  std::vector<double> times;
  for (int i = 0; i < num_runs; ++i) {
    time_clock::time_point t1 = time_clock::now();
    multiply_mv_col_major(matrix, rows, cols, vector, result);
    time_clock::time_point t2 = time_clock::now();

    double elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() /
        1000.0;
    times.push_back(elapsed);
  }

  BenchmarkResult stats = calculate_statistics(times);
  std::cout << "Average time: " << stats.avg_time_ms << " ms" << std::endl;
  std::cout << "Std deviation: " << stats.std_dev_ms << " ms" << std::endl;

  delete[] matrix;
  delete[] vector;
  delete[] result;
}

void benchmark_mm_naive(int size, int num_runs) {
  std::cout << "\n=== Matrix-Matrix (Naive) Benchmark ===" << std::endl;
  std::cout << "Matrix size: " << size << "x" << size << std::endl;
  std::cout << "Number of runs: " << num_runs << std::endl;

  double *matrixA = new double[size * size];
  double *matrixB = new double[size * size];
  double *result = new double[size * size];

  fill_matrix(matrixA, size * size);
  fill_matrix(matrixB, size * size);

  std::vector<double> times;
  for (int i = 0; i < num_runs; ++i) {
    time_clock::time_point t1 = time_clock::now();
    multiply_mm_naive(matrixA, size, size, matrixB, size, size, result);
    time_clock::time_point t2 = time_clock::now();

    double elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() /
        1000.0;
    times.push_back(elapsed);
  }

  BenchmarkResult stats = calculate_statistics(times);
  std::cout << "Average time: " << stats.avg_time_ms << " ms" << std::endl;
  std::cout << "Std deviation: " << stats.std_dev_ms << " ms" << std::endl;

  delete[] matrixA;
  delete[] matrixB;
  delete[] result;
}

void benchmark_mm_transposed_b(int size, int num_runs) {
  std::cout << "\n=== Matrix-Matrix (Transposed B) Benchmark ===" << std::endl;
  std::cout << "Matrix size: " << size << "x" << size << std::endl;
  std::cout << "Number of runs: " << num_runs << std::endl;

  double *matrixA = new double[size * size];
  double *matrixB_transposed = new double[size * size];
  double *result = new double[size * size];

  fill_matrix(matrixA, size * size);
  fill_matrix(matrixB_transposed, size * size);

  std::vector<double> times;
  for (int i = 0; i < num_runs; ++i) {
    time_clock::time_point t1 = time_clock::now();
    multiply_mm_transposed_b(matrixA, size, size, matrixB_transposed, size,
                             size, result);
    time_clock::time_point t2 = time_clock::now();

    double elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() /
        1000.0;
    times.push_back(elapsed);
  }

  BenchmarkResult stats = calculate_statistics(times);
  std::cout << "Average time: " << stats.avg_time_ms << " ms" << std::endl;
  std::cout << "Std deviation: " << stats.std_dev_ms << " ms" << std::endl;

  delete[] matrixA;
  delete[] matrixB_transposed;
  delete[] result;
}

int main() {
  std::srand(42);

  int num_runs = 10;

  std::cout << "===================================" << std::endl;
  std::cout << "Linear Algebra Benchmarking Suite" << std::endl;
  std::cout << "===================================" << std::endl;

  // Small matrices
  benchmark_mv_row_major(100, 100, num_runs);
  benchmark_mv_col_major(100, 100, num_runs);
  benchmark_mm_naive(100, num_runs);
  benchmark_mm_transposed_b(100, num_runs);

  // Medium matrices
  benchmark_mv_row_major(500, 500, num_runs);
  benchmark_mv_col_major(500, 500, num_runs);
  benchmark_mm_naive(500, num_runs);
  benchmark_mm_transposed_b(500, num_runs);

  // Large matrices
  benchmark_mv_row_major(1000, 1000, num_runs);
  benchmark_mv_col_major(1000, 1000, num_runs);
  benchmark_mm_naive(1000, num_runs);
  benchmark_mm_transposed_b(1000, num_runs);

  return 0;
}
