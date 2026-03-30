#include "matrix_operations.h"
#include <stdexcept>

void multiply_mv_row_major(const double* matrix, int rows, int cols, 
                           const double* vector, double* result) {
  if (!matrix || !vector || !result) {
    throw std::invalid_argument("Null pointer passed to multiply_mv_row_major");
  }
  
  // TODO: Implement matrix-vector multiplication (row-major)
}

void multiply_mv_col_major(const double* matrix, int rows, int cols, 
                           const double* vector, double* result) {
  if (!matrix || !vector || !result) {
    throw std::invalid_argument("Null pointer passed to multiply_mv_col_major");
  }
  
  // TODO: Implement matrix-vector multiplication (column-major)
}

void multiply_mm_naive(const double* matrixA, int rowsA, int colsA, 
                      const double* matrixB, int rowsB, int colsB, 
                      double* result) {
  if (!matrixA || !matrixB || !result) {
    throw std::invalid_argument("Null pointer passed to multiply_mm_naive");
  }
  
  if (colsA != rowsB) {
    throw std::invalid_argument("Incompatible matrix dimensions");
  }
  
  // TODO: Implement naive matrix-matrix multiplication
}

void multiply_mm_transposed_b(const double* matrixA, int rowsA, int colsA, 
                             const double* matrixB_transposed, int rowsB, 
                             int colsB, double* result) {
  if (!matrixA || !matrixB_transposed || !result) {
    throw std::invalid_argument("Null pointer passed to multiply_mm_transposed_b");
  }
  
  if (colsA != colsB) {
    throw std::invalid_argument("Incompatible matrix dimensions");
  }
  
  // TODO: Implement matrix-matrix multiplication with transposed B
}
