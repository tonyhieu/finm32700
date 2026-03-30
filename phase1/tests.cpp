#include "matrix_operations.h"
#include <cmath>
#include <iostream>

const double EPSILON = 1e-9;

bool approx_equal(double a, double b) {
  return std::abs(a - b) < EPSILON;
}

void test_multiply_mv_row_major() {
  std::cout << "Testing multiply_mv_row_major..." << std::endl;
  
  // 2x3 matrix (row-major): [[1, 2, 3], [4, 5, 6]]
  // vector: [1, 2, 3]
  // expected result: [1*1+2*2+3*3, 4*1+5*2+6*3] = [14, 32]
  
  double matrix[] = {1, 2, 3, 4, 5, 6};
  double vector[] = {1, 2, 3};
  double result[2] = {0};
  double expected[] = {14, 32};
  
  multiply_mv_row_major(matrix, 2, 3, vector, result);
  
  bool passed = true;
  for (int i = 0; i < 2; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i] 
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }
  
  if (passed) {
    std::cout << "  PASSED" << std::endl;
  }
}

void test_multiply_mv_col_major() {
  std::cout << "Testing multiply_mv_col_major..." << std::endl;
  
  // 2x3 matrix (column-major): [[1, 2, 3], [4, 5, 6]]
  // stored as [1, 4, 2, 5, 3, 6]
  // vector: [1, 2, 3]
  // expected result: [14, 32]
  
  double matrix[] = {1, 4, 2, 5, 3, 6};
  double vector[] = {1, 2, 3};
  double result[2] = {0};
  double expected[] = {14, 32};
  
  multiply_mv_col_major(matrix, 2, 3, vector, result);
  
  bool passed = true;
  for (int i = 0; i < 2; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i] 
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }
  
  if (passed) {
    std::cout << "  PASSED" << std::endl;
  }
}

void test_multiply_mm_naive() {
  std::cout << "Testing multiply_mm_naive..." << std::endl;
  
  // 2x2 matrices: A = [[1, 2], [3, 4]], B = [[5, 6], [7, 8]]
  // result = [[1*5+2*7, 1*6+2*8], [3*5+4*7, 3*6+4*8]] = [[19, 22], [43, 50]]
  
  double matrixA[] = {1, 2, 3, 4};
  double matrixB[] = {5, 6, 7, 8};
  double result[4] = {0};
  double expected[] = {19, 22, 43, 50};
  
  multiply_mm_naive(matrixA, 2, 2, matrixB, 2, 2, result);
  
  bool passed = true;
  for (int i = 0; i < 4; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i] 
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }
  
  if (passed) {
    std::cout << "  PASSED" << std::endl;
  }
}

void test_multiply_mm_transposed_b() {
  std::cout << "Testing multiply_mm_transposed_b..." << std::endl;
  
  // A = [[1, 2], [3, 4]], B = [[5, 6], [7, 8]], B^T = [[5, 7], [6, 8]]
  // stored as [5, 7, 6, 8]
  // result = [[19, 22], [43, 50]]
  
  double matrixA[] = {1, 2, 3, 4};
  double matrixB_transposed[] = {5, 7, 6, 8};
  double result[4] = {0};
  double expected[] = {19, 22, 43, 50};
  
  multiply_mm_transposed_b(matrixA, 2, 2, matrixB_transposed, 2, 2, result);
  
  bool passed = true;
  for (int i = 0; i < 4; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i] 
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }
  
  if (passed) {
    std::cout << "  PASSED" << std::endl;
  }
}

int main() {
  std::cout << "==================" << std::endl;
  std::cout << "Running Test Suite" << std::endl;
  std::cout << "==================" << std::endl;
  
  test_multiply_mv_row_major();
  test_multiply_mv_col_major();
  test_multiply_mm_naive();
  test_multiply_mm_transposed_b();
  
  return 0;
}
