#ifndef MATRIX_OPERATIONS_OPTIMIZED_H
#define MATRIX_OPERATIONS_OPTIMIZED_H

// Optimized matrix multiplication using cache blocking/tiling
void multiply_mm_blocked(const double *matrixA, int rowsA, int colsA,
                        const double *matrixB, int rowsB, int colsB,
                        double *result);

#endif // MATRIX_OPERATIONS_OPTIMIZED_H
