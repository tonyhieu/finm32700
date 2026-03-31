#include "matrix_operations.h"
#include <stdexcept>

void multiply_mv_row_major(const double *matrix, int rows, int cols,
                           const double *vector, double *result) {
  if (!matrix || !vector || !result) {
    throw std::invalid_argument("Null pointer passed to multiply_mv_row_major");
  }

  double curr;
  int i = 0;
  for (int r = 0; r < rows; ++r) {
    curr = 0;
    for (int c = 0; c < cols; ++c) {
      curr += matrix[r * cols + c] * vector[c];
    }
    result[i++] = curr;
  }
}

void multiply_mv_col_major(const double *matrix, int rows, int cols,
                           const double *vector, double *result) {
  if (!matrix || !vector || !result) {
    throw std::invalid_argument("Null pointer passed to multiply_mv_col_major");
  }

  double curr;
  int i = 0;
  for (int r = 0; r < rows; ++r) {
    curr = 0;
    for (int c = 0; c < cols; ++c) {
      curr += matrix[r + c * rows] * vector[c];
    }
    result[i++] = curr;
  }
}

void multiply_mm_naive(const double *matrixA, int rowsA, int colsA,
                       const double *matrixB, int rowsB, int colsB,
                       double *result) {
  if (!matrixA || !matrixB || !result) {
    throw std::invalid_argument("Null pointer passed to multiply_mm_naive");
  }

  if (colsA != rowsB) {
    throw std::invalid_argument("Incompatible matrix dimensions");
  }

  double *col_b = new double[rowsB];
  double *col_result = new double[rowsA];

  for (int c = 0; c < colsB; ++c) {
    for (int r = 0; r < rowsB; ++r) {
      col_b[r] = matrixB[r * colsB + c];
    }

    multiply_mv_row_major(matrixA, rowsA, colsA, col_b, col_result);

    for (int r = 0; r < rowsA; ++r) {
      result[r * colsB + c] = col_result[r];
    }
  }

  delete[] col_b;
  delete[] col_result;
}

void multiply_mm_transposed_b(const double *matrixA, int rowsA, int colsA,
                              const double *matrixB_transposed, int rowsB,
                              int colsB, double *result) {
  if (!matrixA || !matrixB_transposed || !result) {
    throw std::invalid_argument(
        "Null pointer passed to multiply_mm_transposed_b");
  }

  if (colsA != colsB) {
    throw std::invalid_argument("Incompatible matrix dimensions");
  }

  // Compute A * B where B is provided in transposed form
  // A is rowsA × colsA
  // Original B is rowsB × colsB
  // B^T (what we're given) is colsB × rowsB
  // We want to compute A × B (not A × B^T!)
  // Result is rowsA × colsB
  // result[i,j] = sum over k of A[i,k] * B[k,j]
  // Since we have B^T, B[k,j] = B^T[j,k]
  // B^T in row-major: B^T[j,k] = matrixB_transposed[j * rowsB + k]

  for (int i = 0; i < rowsA; ++i) {
    for (int j = 0; j < colsB; ++j) {
      double sum = 0;
      for (int k = 0; k < colsA; ++k) {
        sum += matrixA[i * colsA + k] * matrixB_transposed[j * rowsB + k];
      }
      result[i * colsB + j] = sum;
    }
  }
}
