#include "matrix_operations.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

// Simple profiling program to analyze performance with xctrace
// This program runs matrix operations multiple times to generate
// enough data for meaningful profiling results

void run_matrix_vector_operations(int size, int iterations) {
    std::cout << "Running matrix-vector operations (" << size << "x" << size 
              << ", " << iterations << " iterations)..." << std::endl;
    
    // Allocate matrices
    double *matrix = new double[size * size];
    double *vector = new double[size];
    double *result = new double[size];
    
    // Initialize with random data
    for (int i = 0; i < size * size; ++i) {
        matrix[i] = (double)rand() / RAND_MAX;
    }
    for (int i = 0; i < size; ++i) {
        vector[i] = (double)rand() / RAND_MAX;
    }
    
    // Run row-major operations
    for (int iter = 0; iter < iterations; ++iter) {
        multiply_mv_row_major(matrix, size, size, vector, result);
    }
    
    // Run column-major operations
    for (int iter = 0; iter < iterations; ++iter) {
        multiply_mv_col_major(matrix, size, size, vector, result);
    }
    
    // Prevent compiler from optimizing away the computation
    double sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += result[i];
    }
    if (sum < -1e100) std::cout << sum << std::endl; // Never true, but compiler doesn't know
    
    delete[] matrix;
    delete[] vector;
    delete[] result;
    
    std::cout << "  Matrix-vector operations completed." << std::endl;
}

void run_matrix_matrix_operations(int size, int iterations) {
    std::cout << "Running matrix-matrix operations (" << size << "x" << size 
              << ", " << iterations << " iterations)..." << std::endl;
    
    // Allocate matrices
    double *A = new double[size * size];
    double *B = new double[size * size];
    double *B_T = new double[size * size];
    double *C = new double[size * size];
    
    // Initialize with random data
    for (int i = 0; i < size * size; ++i) {
        A[i] = (double)rand() / RAND_MAX;
        B[i] = (double)rand() / RAND_MAX;
    }
    
    // Create transpose of B
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            B_T[j * size + i] = B[i * size + j];
        }
    }
    
    // Run naive matrix multiplication
    for (int iter = 0; iter < iterations; ++iter) {
        multiply_mm_naive(A, size, size, B, size, size, C);
    }
    
    // Run transposed B matrix multiplication
    for (int iter = 0; iter < iterations; ++iter) {
        multiply_mm_transposed_b(A, size, size, B_T, size, size, C);
    }
    
    // Prevent compiler from optimizing away the computation
    double sum = 0;
    for (int i = 0; i < size * size; ++i) {
        sum += C[i];
    }
    if (sum < -1e100) std::cout << sum << std::endl; // Never true, but compiler doesn't know
    
    delete[] A;
    delete[] B;
    delete[] B_T;
    delete[] C;
    
    std::cout << "  Matrix-matrix operations completed." << std::endl;
}

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "      Matrix Operations Profiling       " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "\nThis program performs matrix operations for profiling." << std::endl;
    std::cout << "Use xctrace to analyze performance:\n" << std::endl;
    std::cout << "  xctrace record --template 'Time Profiler' --launch ./profile" << std::endl;
    std::cout << "\nOr use Instruments.app for GUI-based profiling.\n" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    srand(time(nullptr));
    
    // Run operations with different sizes and iteration counts
    // Smaller matrices run more iterations to accumulate time
    
    std::cout << "\n[1/5] Warming up with small matrices..." << std::endl;
    run_matrix_vector_operations(100, 100);
    run_matrix_matrix_operations(64, 50);
    
    std::cout << "\n[2/5] Medium-size matrix-vector operations..." << std::endl;
    run_matrix_vector_operations(500, 50);
    
    std::cout << "\n[3/5] Medium-size matrix-matrix operations..." << std::endl;
    run_matrix_matrix_operations(128, 20);
    
    std::cout << "\n[4/5] Large matrix-vector operations..." << std::endl;
    run_matrix_vector_operations(1000, 20);
    
    std::cout << "\n[5/5] Large matrix-matrix operations..." << std::endl;
    run_matrix_matrix_operations(256, 10);
    
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Profiling workload completed!" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    return 0;
}
