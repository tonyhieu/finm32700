#include "matrix_operations_optimized.h"
#include <algorithm>
#include <cstring>
#include <stdexcept>

void multiply_mm_blocked(const double *matrixA, int rowsA, int colsA,
                         const double *matrixB, int rowsB, int colsB,
                         double *result) {
  if (!matrixA || !matrixB || !result) {
    throw std::invalid_argument("Null pointer passed to multiply_mm_blocked");
  }

  if (colsA != rowsB) {
    throw std::invalid_argument("Incompatible matrix dimensions");
  }

  // Smaller block size for better cache utilization
  // L1 cache is typically 32KB, we want 3 blocks to fit
  const int BLOCK_SIZE = 32;

  // Initialize result to zero
  std::memset(result, 0, rowsA * colsB * sizeof(double));

  // Outer loops: iterate over blocks/tiles
  // Changed loop order: i, k, j instead of i, j, k for better cache behavior
  for (int ii = 0; ii < rowsA; ii += BLOCK_SIZE) {
    for (int kk = 0; kk < colsA; kk += BLOCK_SIZE) {
      for (int jj = 0; jj < colsB; jj += BLOCK_SIZE) {

        // Calculate bounds for this block (handle edge cases)
        int i_max = std::min(ii + BLOCK_SIZE, rowsA);
        int j_max = std::min(jj + BLOCK_SIZE, colsB);
        int k_max = std::min(kk + BLOCK_SIZE, colsA);

        // Inner loops: compute within this tile
        // i-k-j order for better cache locality
        for (int i = ii; i < i_max; ++i) {
          for (int k = kk; k < k_max; ++k) {
            // Load A[i,k] once and reuse for entire row of B
            double a_val = matrixA[i * colsA + k];

            // Vectorizable inner loop - compiler can optimize this well
            for (int j = jj; j < j_max; ++j) {
              result[i * colsB + j] += a_val * matrixB[k * colsB + j];
            }
          }
        }
      }
    }
  }
}
