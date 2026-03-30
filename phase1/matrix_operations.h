#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

// Matrix-vector multiplication with row-major storage
void multiply_mv_row_major(const double* matrix, int rows, int cols, 
                           const double* vector, double* result);

// Matrix-vector multiplication with column-major storage
void multiply_mv_col_major(const double* matrix, int rows, int cols, 
                           const double* vector, double* result);

// Naive matrix-matrix multiplication
void multiply_mm_naive(const double* matrixA, int rowsA, int colsA, 
                      const double* matrixB, int rowsB, int colsB, 
                      double* result);

// Matrix-matrix multiplication with transposed B
void multiply_mm_transposed_b(const double* matrixA, int rowsA, int colsA, 
                             const double* matrixB_transposed, int rowsB, 
                             int colsB, double* result);

#endif
