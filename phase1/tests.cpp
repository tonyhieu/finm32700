#include "matrix_operations.h"
#include "matrix_operations_optimized.h"
#include <cmath>
#include <iostream>
#include <string>

const double EPSILON = 1e-9;
int tests_passed = 0;
int tests_failed = 0;

bool approx_equal(double a, double b) { return std::abs(a - b) < EPSILON; }

bool compare_arrays(const double *arr1, const double *arr2, int size, double epsilon = EPSILON) {
  for (int i = 0; i < size; ++i) {
    if (std::abs(arr1[i] - arr2[i]) > epsilon) {
      return false;
    }
  }
  return true;
}

void report_test(const std::string &test_name, bool passed) {
  if (passed) {
    std::cout << "  ✓ PASSED" << std::endl;
    tests_passed++;
  } else {
    std::cout << "  ✗ FAILED" << std::endl;
    tests_failed++;
  }
}

void test_multiply_mv_row_major_basic() {
  std::cout << "Test 1: multiply_mv_row_major - basic 2x3 matrix" << std::endl;

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

  report_test("multiply_mv_row_major_basic", passed);
}

void test_multiply_mv_row_major_square() {
  std::cout << "Test 2: multiply_mv_row_major - square 3x3 matrix" << std::endl;

  // 3x3 matrix: [[1, 0, 0], [0, 1, 0], [0, 0, 1]] (identity)
  // vector: [5, 7, 9]
  // expected: [5, 7, 9]

  double matrix[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  double vector[] = {5, 7, 9};
  double result[3] = {0};
  double expected[] = {5, 7, 9};

  multiply_mv_row_major(matrix, 3, 3, vector, result);

  bool passed = true;
  for (int i = 0; i < 3; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mv_row_major_square", passed);
}

void test_multiply_mv_row_major_1x1() {
  std::cout << "Test 3: multiply_mv_row_major - 1x1 matrix (edge case)" << std::endl;

  double matrix[] = {5};
  double vector[] = {3};
  double result[1] = {0};
  double expected[] = {15};

  multiply_mv_row_major(matrix, 1, 1, vector, result);

  bool passed = approx_equal(result[0], expected[0]);
  if (!passed) {
    std::cout << "  FAILED: result = " << result[0] << ", expected " << expected[0] << std::endl;
  }

  report_test("multiply_mv_row_major_1x1", passed);
}

void test_multiply_mv_row_major_tall() {
  std::cout << "Test 4: multiply_mv_row_major - tall 4x2 matrix" << std::endl;

  // 4x2 matrix: [[1, 2], [3, 4], [5, 6], [7, 8]]
  // vector: [2, 3]
  // expected: [1*2+2*3, 3*2+4*3, 5*2+6*3, 7*2+8*3] = [8, 18, 28, 38]

  double matrix[] = {1, 2, 3, 4, 5, 6, 7, 8};
  double vector[] = {2, 3};
  double result[4] = {0};
  double expected[] = {8, 18, 28, 38};

  multiply_mv_row_major(matrix, 4, 2, vector, result);

  bool passed = true;
  for (int i = 0; i < 4; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mv_row_major_tall", passed);
}

void test_multiply_mv_col_major_basic() {
  std::cout << "Test 5: multiply_mv_col_major - basic 2x3 matrix" << std::endl;

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

  report_test("multiply_mv_col_major_basic", passed);
}

void test_multiply_mv_col_major_identity() {
  std::cout << "Test 6: multiply_mv_col_major - 3x3 identity matrix" << std::endl;

  // 3x3 identity (column-major): stored as [1, 0, 0, 0, 1, 0, 0, 0, 1]
  // vector: [2, 4, 6]
  // expected: [2, 4, 6]

  double matrix[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  double vector[] = {2, 4, 6};
  double result[3] = {0};
  double expected[] = {2, 4, 6};

  multiply_mv_col_major(matrix, 3, 3, vector, result);

  bool passed = true;
  for (int i = 0; i < 3; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mv_col_major_identity", passed);
}

void test_multiply_mm_naive_square() {
  std::cout << "Test 7: multiply_mm_naive - square 2x2 matrices" << std::endl;

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

  report_test("multiply_mm_naive_square", passed);
}

void test_multiply_mm_naive_nonsquare() {
  std::cout << "Test 8: multiply_mm_naive - non-square 2x3 * 3x2" << std::endl;

  // A is 2x3: [[1, 2, 3], [4, 5, 6]]
  // B is 3x2: [[1, 2], [3, 4], [5, 6]]
  // Result is 2x2:
  // C[0,0] = 1*1+2*3+3*5 = 22, C[0,1] = 1*2+2*4+3*6 = 28
  // C[1,0] = 4*1+5*3+6*5 = 49, C[1,1] = 4*2+5*4+6*6 = 64

  double matrixA[] = {1, 2, 3, 4, 5, 6};
  double matrixB[] = {1, 2, 3, 4, 5, 6};
  double result[4] = {0};
  double expected[] = {22, 28, 49, 64};

  multiply_mm_naive(matrixA, 2, 3, matrixB, 3, 2, result);

  bool passed = true;
  for (int i = 0; i < 4; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mm_naive_nonsquare", passed);
}

void test_multiply_mm_naive_identity() {
  std::cout << "Test 9: multiply_mm_naive - identity matrix" << std::endl;

  // A = [[1, 2], [3, 4]], I = [[1, 0], [0, 1]]
  // A * I should equal A

  double matrixA[] = {1, 2, 3, 4};
  double matrixI[] = {1, 0, 0, 1};
  double result[4] = {0};
  double expected[] = {1, 2, 3, 4};

  multiply_mm_naive(matrixA, 2, 2, matrixI, 2, 2, result);

  bool passed = true;
  for (int i = 0; i < 4; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mm_naive_identity", passed);
}

void test_multiply_mm_naive_1x1() {
  std::cout << "Test 10: multiply_mm_naive - 1x1 matrices (edge case)" << std::endl;

  double matrixA[] = {7};
  double matrixB[] = {3};
  double result[1] = {0};
  double expected[] = {21};

  multiply_mm_naive(matrixA, 1, 1, matrixB, 1, 1, result);

  bool passed = approx_equal(result[0], expected[0]);
  if (!passed) {
    std::cout << "  FAILED: result = " << result[0] << ", expected " << expected[0] << std::endl;
  }

  report_test("multiply_mm_naive_1x1", passed);
}

void test_multiply_mm_naive_wide() {
  std::cout << "Test 11: multiply_mm_naive - wide matrices 2x3 * 3x4" << std::endl;

  // A is 2x3: [[1, 2, 3], [4, 5, 6]]
  // B is 3x4: [[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]]
  // Result is 2x4

  double matrixA[] = {1, 2, 3, 4, 5, 6};
  double matrixB[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  double result[8] = {0};
  // C[0,0] = 1*1+2*5+3*9 = 38, C[0,1] = 1*2+2*6+3*10 = 44
  // C[0,2] = 1*3+2*7+3*11 = 50, C[0,3] = 1*4+2*8+3*12 = 56
  // C[1,0] = 4*1+5*5+6*9 = 83, C[1,1] = 4*2+5*6+6*10 = 98
  // C[1,2] = 4*3+5*7+6*11 = 113, C[1,3] = 4*4+5*8+6*12 = 128
  double expected[] = {38, 44, 50, 56, 83, 98, 113, 128};

  multiply_mm_naive(matrixA, 2, 3, matrixB, 3, 4, result);

  bool passed = true;
  for (int i = 0; i < 8; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mm_naive_wide", passed);
}

void test_multiply_mm_transposed_b_basic() {
  std::cout << "Test 12: multiply_mm_transposed_b - basic 2x2 matrices" << std::endl;

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

  report_test("multiply_mm_transposed_b_basic", passed);
}

void test_multiply_mm_transposed_b_nonsquare() {
  std::cout << "Test 13: multiply_mm_transposed_b - 2x3 * 3x3 (B given as B^T)" << std::endl;

  // Computing A × B where B is provided as B^T:
  // A is 2x3: [[1, 2, 3], [4, 5, 6]]
  // B is 3x3: [[1, 0, 0], [0, 1, 0], [0, 0, 1]] (identity)
  // B^T is 3x3: [[1, 0, 0], [0, 1, 0], [0, 0, 1]] stored as [1, 0, 0, 0, 1, 0, 0, 0, 1]
  // Result = A × B = A × I = A is 2x3:
  // C = [[1, 2, 3], [4, 5, 6]]

  double matrixA[] = {1, 2, 3, 4, 5, 6};
  double matrixB_transposed[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  double result[6] = {0};
  double expected[] = {1, 2, 3, 4, 5, 6};

  // Original B is 3x3, so rowsB=3, colsB=3
  multiply_mm_transposed_b(matrixA, 2, 3, matrixB_transposed, 3, 3, result);

  bool passed = true;
  for (int i = 0; i < 6; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mm_transposed_b_nonsquare", passed);
}

void test_multiply_mm_transposed_b_identity() {
  std::cout << "Test 14: multiply_mm_transposed_b - identity matrix" << std::endl;

  // A = [[1, 2], [3, 4]], I^T = I = [[1, 0], [0, 1]]
  // A * I should equal A

  double matrixA[] = {1, 2, 3, 4};
  double matrixI_transposed[] = {1, 0, 0, 1};
  double result[4] = {0};
  double expected[] = {1, 2, 3, 4};

  multiply_mm_transposed_b(matrixA, 2, 2, matrixI_transposed, 2, 2, result);

  bool passed = true;
  for (int i = 0; i < 4; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mm_transposed_b_identity", passed);
}

void test_multiply_mm_transposed_b_3x3() {
  std::cout << "Test 15: multiply_mm_transposed_b - 3x3 matrices" << std::endl;

  // A = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
  // B = [[1, 0, 1], [0, 1, 0], [1, 0, 1]]
  // B^T = [[1, 0, 1], [0, 1, 0], [1, 0, 1]] (symmetric)
  // Result:
  // [0,0] = 1*1+2*0+3*1 = 4, [0,1] = 1*0+2*1+3*0 = 2, [0,2] = 1*1+2*0+3*1 = 4
  // [1,0] = 4*1+5*0+6*1 = 10, [1,1] = 4*0+5*1+6*0 = 5, [1,2] = 4*1+5*0+6*1 = 10
  // [2,0] = 7*1+8*0+9*1 = 16, [2,1] = 7*0+8*1+9*0 = 8, [2,2] = 7*1+8*0+9*1 = 16

  double matrixA[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  double matrixB_transposed[] = {1, 0, 1, 0, 1, 0, 1, 0, 1};
  double result[9] = {0};
  double expected[] = {4, 2, 4, 10, 5, 10, 16, 8, 16};

  multiply_mm_transposed_b(matrixA, 3, 3, matrixB_transposed, 3, 3, result);

  bool passed = true;
  for (int i = 0; i < 9; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mm_transposed_b_3x3", passed);
}

// ========== OPTIMIZED BLOCKED MATRIX MULTIPLICATION TESTS ==========

// Test multiply_mm_blocked - basic 2x2 matrices
void test_multiply_mm_blocked_basic() {
  std::cout << "Test 16: multiply_mm_blocked - basic 2x2 matrices" << std::endl;
  
  double matrixA[] = {1, 2, 3, 4};  // 2x2
  double matrixB[] = {5, 6, 7, 8};  // 2x2
  double result[4] = {0};

  multiply_mm_blocked(matrixA, 2, 2, matrixB, 2, 2, result);

  double expected[] = {19, 22, 43, 50};

  report_test("multiply_mm_blocked - basic 2x2", compare_arrays(result, expected, 4));
}

// Test multiply_mm_blocked - non-square matrices
void test_multiply_mm_blocked_nonsquare() {
  std::cout << "Test 17: multiply_mm_blocked - non-square 2x3 * 3x2" << std::endl;
  
  double matrixA[] = {1, 2, 3, 4, 5, 6};  // 2x3
  double matrixB[] = {7, 8, 9, 10, 11, 12};  // 3x2
  double result[4] = {0};

  multiply_mm_blocked(matrixA, 2, 3, matrixB, 3, 2, result);

  double expected[] = {58, 64, 139, 154};

  report_test("multiply_mm_blocked - non-square", compare_arrays(result, expected, 4));
}

// Test multiply_mm_blocked - identity matrix
void test_multiply_mm_blocked_identity() {
  std::cout << "Test 18: multiply_mm_blocked - identity matrix" << std::endl;
  
  double matrixA[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};  // 3x3
  double identity[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};  // 3x3 identity
  double result[9] = {0};

  multiply_mm_blocked(matrixA, 3, 3, identity, 3, 3, result);

  report_test("multiply_mm_blocked - identity", compare_arrays(result, matrixA, 9));
}

// Test multiply_mm_blocked - 1x1 edge case
void test_multiply_mm_blocked_1x1() {
  std::cout << "Test 19: multiply_mm_blocked - 1x1 matrices (edge case)" << std::endl;
  
  double matrixA[] = {5.0};
  double matrixB[] = {3.0};
  double result[1] = {0};

  multiply_mm_blocked(matrixA, 1, 1, matrixB, 1, 1, result);

  double expected[] = {15.0};

  report_test("multiply_mm_blocked - 1x1", compare_arrays(result, expected, 1));
}

// Test multiply_mm_blocked - larger matrix (64x64) for block size testing
void test_multiply_mm_blocked_large() {
  std::cout << "Test 20: multiply_mm_blocked - 64x64 matrix (block boundary test)" << std::endl;
  
  const int N = 64;
  double *A = new double[N * N];
  double *B = new double[N * N];
  double *result = new double[N * N];
  double *expected = new double[N * N];

  // Initialize with simple pattern
  for (int i = 0; i < N * N; ++i) {
    A[i] = (i % 10) + 1.0;
    B[i] = ((i * 2) % 10) + 1.0;
  }

  // Compute expected result with naive implementation
  multiply_mm_naive(A, N, N, B, N, N, expected);

  // Compute with blocked implementation
  multiply_mm_blocked(A, N, N, B, N, N, result);

  bool passed = compare_arrays(result, expected, N * N);

  delete[] A;
  delete[] B;
  delete[] result;
  delete[] expected;

  report_test("multiply_mm_blocked - 64x64", passed);
}

// Test multiply_mm_blocked vs naive - verify consistency across multiple sizes
void test_multiply_mm_blocked_vs_naive() {
  std::cout << "Test 21: multiply_mm_blocked - vs naive (sizes 33, 65, 100)" << std::endl;
  
  const int sizes[] = {33, 65, 100};  // Test non-aligned and aligned sizes
  bool all_passed = true;

  for (int size : sizes) {
    double *A = new double[size * size];
    double *B = new double[size * size];
    double *result_naive = new double[size * size];
    double *result_blocked = new double[size * size];

    // Initialize with semi-random pattern
    for (int i = 0; i < size * size; ++i) {
      A[i] = ((i * 3) % 17) / 7.0;
      B[i] = ((i * 5) % 13) / 11.0;
    }

    multiply_mm_naive(A, size, size, B, size, size, result_naive);
    multiply_mm_blocked(A, size, size, B, size, size, result_blocked);

    bool passed = compare_arrays(result_naive, result_blocked, size * size, 1e-9);
    
    if (!passed) {
      std::cout << "  Failed at size " << size << std::endl;
      all_passed = false;
    }

    delete[] A;
    delete[] B;
    delete[] result_naive;
    delete[] result_blocked;
  }

  report_test("multiply_mm_blocked - vs naive", all_passed);
}

void test_multiply_mm_transposed_b_wide() {
  std::cout << "Test 16: multiply_mm_transposed_b - 3x2 * 2x4 (B given as B^T)" << std::endl;

  // Computing A × B where B is provided as B^T:
  // A is 3x2: [[1, 2], [3, 4], [5, 6]]
  // B is 2x4: [[1, 2, 3, 4], [5, 6, 7, 8]]
  // B^T is 4x2: [[1, 5], [2, 6], [3, 7], [4, 8]]
  //   stored as [1, 5, 2, 6, 3, 7, 4, 8]
  // Result = A × B is 3x4:
  // [0,0] = 1*1+2*5 = 11, [0,1] = 1*2+2*6 = 14, [0,2] = 1*3+2*7 = 17, [0,3] = 1*4+2*8 = 20
  // [1,0] = 3*1+4*5 = 23, [1,1] = 3*2+4*6 = 30, [1,2] = 3*3+4*7 = 37, [1,3] = 3*4+4*8 = 44
  // [2,0] = 5*1+6*5 = 35, [2,1] = 5*2+6*6 = 46, [2,2] = 5*3+6*7 = 57, [2,3] = 5*4+6*8 = 68

  double matrixA[] = {1, 2, 3, 4, 5, 6};
  double matrixB_transposed[] = {1, 5, 2, 6, 3, 7, 4, 8};
  double result[12] = {0};
  double expected[] = {11, 14, 17, 20, 23, 30, 37, 44, 35, 46, 57, 68};

  // Original B is 2x4, so rowsB=2, colsB=4
  multiply_mm_transposed_b(matrixA, 3, 2, matrixB_transposed, 2, 4, result);

  bool passed = true;
  for (int i = 0; i < 12; ++i) {
    if (!approx_equal(result[i], expected[i])) {
      std::cout << "  FAILED: result[" << i << "] = " << result[i]
                << ", expected " << expected[i] << std::endl;
      passed = false;
    }
  }

  report_test("multiply_mm_transposed_b_wide", passed);
}

int main() {
  std::cout << "======================================" << std::endl;
  std::cout << "   Matrix Operations Test Suite" << std::endl;
  std::cout << "======================================" << std::endl;
  std::cout << std::endl;

  // Matrix-vector multiplication tests (row-major)
  std::cout << "--- Matrix-Vector (Row-Major) Tests ---" << std::endl;
  test_multiply_mv_row_major_basic();
  test_multiply_mv_row_major_square();
  test_multiply_mv_row_major_1x1();
  test_multiply_mv_row_major_tall();
  std::cout << std::endl;

  // Matrix-vector multiplication tests (column-major)
  std::cout << "--- Matrix-Vector (Column-Major) Tests ---" << std::endl;
  test_multiply_mv_col_major_basic();
  test_multiply_mv_col_major_identity();
  std::cout << std::endl;

  // Matrix-matrix multiplication tests (naive)
  std::cout << "--- Matrix-Matrix (Naive) Tests ---" << std::endl;
  test_multiply_mm_naive_square();
  test_multiply_mm_naive_nonsquare();
  test_multiply_mm_naive_identity();
  test_multiply_mm_naive_1x1();
  test_multiply_mm_naive_wide();
  std::cout << std::endl;

  // Matrix-matrix multiplication tests (transposed B)
  std::cout << "--- Matrix-Matrix (Transposed B) Tests ---" << std::endl;
  test_multiply_mm_transposed_b_basic();
  test_multiply_mm_transposed_b_nonsquare();
  test_multiply_mm_transposed_b_identity();
  test_multiply_mm_transposed_b_3x3();
  std::cout << std::endl;

  // Matrix-matrix multiplication tests (blocked/optimized)
  std::cout << "--- Matrix-Matrix (Blocked/Optimized) Tests ---" << std::endl;
  test_multiply_mm_blocked_basic();
  test_multiply_mm_blocked_nonsquare();
  test_multiply_mm_blocked_identity();
  test_multiply_mm_blocked_1x1();
  test_multiply_mm_blocked_large();
  test_multiply_mm_blocked_vs_naive();
  std::cout << std::endl;

  // Summary
  std::cout << "======================================" << std::endl;
  std::cout << "Test Results: " << tests_passed << " passed, " << tests_failed << " failed" << std::endl;
  std::cout << "======================================" << std::endl;

  return tests_failed > 0 ? 1 : 0;
}
