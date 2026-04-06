# Phase 1 Report: High-Performance Linear Algebra Kernels

## Part 2: Performance Analysis and Optimization

### 1. Benchmarking Framework

We developed a benchmarking framework using `std::chrono` for high-resolution timing. Each benchmark runs multiple iterations (5-20 depending on the operation's cost) and computes statistical measures including mean execution time, standard deviation, minimum, and maximum. This approach accounts for system noise and ensures measurement reliability.

### 2. Cache Locality Analysis

#### 2.1 Matrix-Vector Multiplication: Row-Major vs Column-Major

We tested square matrices ranging from 100×100 to 2000×2000, comparing row-major against column-major storage:

| Size | Row-Major (ms) | Column-Major (ms) | Speedup |
|------|----------------|-------------------|---------|
| 100  | 0.0061        | 0.0220           | 3.63x   |
| 500  | 0.2033        | 0.4432           | 2.18x   |
| 1000 | 0.6952        | 1.3624           | 1.96x   |
| 2000 | 2.1883        | 4.1821           | 1.91x   |

Row-major storage is consistently 1.9-3.6x faster than column-major. This performance difference is due to spatial locality: when accessing a row of a row-major matrix, consecutive elements are stored adjacently in memory. When the CPU fetches a cache line (64 bytes, or 8 doubles), all 8 values are used before the next cache line is needed. Column-major storage stores columns contiguously, so row access jumps by `rows` elements each time. This loads cache lines but only uses one value from each, wasting the other 7 slots. The hardware prefetcher can predict sequential access patterns in row-major layout but struggles with the large strides in column-major access.

#### 2.2 Matrix-Matrix Multiplication: Naive vs Transposed B

For matrix-matrix multiplication, we compared the naive implementation against a version that takes B in transposed form:

| Size | Naive (ms) | Transposed B (ms) | Speedup |
|------|------------|-------------------|---------|
| 64   | 0.1925    | 0.1584           | 1.22x   |
| 128  | 1.1855    | 2.2491           | 0.53x   |
| 256  | 9.2796    | 5.5776           | 1.66x   |
| 512  | 62.5985   | 53.5158          | 1.17x   |

The results vary with matrix size. For small matrices (64×64), the transposed version is 1.22x faster since both matrices fit in L1 cache and benefit from the row-row access pattern. At 128×128, the naive implementation is faster because it uses `multiply_mv_row_major` which handles column extraction efficiently at this size. For larger matrices (256×256 and above), the transposed version is 1.17-1.66x faster because the row-row dot product pattern has better cache locality than accessing B column-wise.

The key difference is the memory access pattern: the naive implementation accesses A row-wise but B column-wise (jumping `colsB` elements at a time), while the transposed version accesses both A and B^T row-wise.

### 3. Memory Alignment

We tested whether aligning memory to 64-byte boundaries (matching typical cache line size) would improve performance:

| Size | Unaligned (ms) | 64-byte Aligned (ms) | Speedup |
|------|----------------|----------------------|---------|
| 256  | 5.2435        | 5.1680              | 1.01x   |
| 512  | 51.6713       | 51.2517             | 1.01x   |
| 1024 | 490.6303      | 500.3716            | 0.98x   |

The results show minimal impact — alignment provides essentially no benefit (±1-2%). Modern x86-64 CPUs handle unaligned loads efficiently in hardware. Additionally, the implementation does not use explicit SIMD instructions; alignment matters most for operations like `_mm256_load_pd` which require 32-byte alignment. The default `new` operator already provides 8 or 16-byte alignment for doubles, which is sufficient for scalar operations. The 64-byte alignment would matter more with SIMD vectorization (for aligned loads) or multi-threading (to prevent false sharing where different threads' data ends up on the same cache line).

Alignment had minimal impact for this implementation, but would become important with SIMD vectorization or parallelization.

### 4. Compiler Optimization Impact

We compiled the implementations at both -O0 (no optimization) and -O3 (aggressive optimization) to measure compiler impact.

#### 4.1 Matrix-Vector Multiplication

**At -O0 (no optimization)**:

| Size | Row-Major (ms) | Col-Major (ms) |
|------|----------------|----------------|
| 100  | 0.0105        | 0.0109        |
| 500  | 0.2768        | 0.2941        |
| 1000 | 1.1590        | 1.1837        |
| 2000 | 4.4057        | 6.7630        |

**Results at -O3**:

| Size | Row-Major (ms) | Col-Major (ms) |
|------|----------------|----------------|
| 100  | 0.0025        | 0.0046        |
| 500  | 0.1036        | 0.1790        |
| 1000 | 0.4890        | 0.8116        |
| 2000 | 2.2369        | 3.5446        |

**Speedup from -O3**:

| Size | Row-Major | Col-Major |
|------|-----------|-----------|
| 100  | 4.20x    | 2.37x    |
| 500  | 2.67x    | 1.64x    |
| 1000 | 2.37x    | 1.46x    |
| 2000 | 1.97x    | 1.91x    |

The compiler optimization provides 1.5-4.2x speedups. Row-major operations benefit more (up to 4.2x for small sizes) because the sequential access pattern provides more opportunities for compiler optimization, particularly vectorization.

#### 4.2 Matrix-Matrix Multiplication

The matrix-matrix results show even larger speedups:

**Speedup from -O3**:

| Size | Naive | Transposed B |
|------|-------|--------------|
| 64   | 5.64x | 6.33x       |
| 128  | 3.90x | 9.36x       |
| 256  | 2.73x | 3.69x       |
| 512  | 2.40x | 2.91x       |

The largest speedup is 9.36x for transposed B at 128×128, where the compiler effectively optimizes the row-row dot product pattern, likely including auto-vectorization. The speedup decreases for larger matrices as memory bandwidth becomes the bottleneck rather than computation.

The -O3 optimizations include: loop unrolling to reduce overhead, instruction scheduling to minimize CPU stalls, register allocation to reduce memory loads, elimination of redundant computations, potential use of SIMD instructions, and automatic function inlining to remove jump instructions.

Even with -O3, cache-friendly patterns matter. Row-major still beats column-major, and transposed B still helps for larger matrices. Compiler optimization amplifies good algorithm design but does not replace it.

### 5. Understanding Cache Behavior

Modern CPUs have multiple levels of cache with different speeds and sizes. L1 cache is closest to the CPU (32-64 KB, ~4 cycles to access), L2 is further (256-512 KB, ~12 cycles), and L3 is shared across cores (8-32 MB, ~40 cycles). Main memory is large but slow (~200 cycles). Performance depends on keeping frequently accessed data in L1 cache.

**Temporal locality** means recently accessed data is likely to be accessed again soon. **Spatial locality** means data near recently accessed data is likely to be accessed soon. The optimizations exploit both: row-major access exploits spatial locality (accessing consecutive elements), and cache blocking exploits temporal locality (reusing data while it remains in cache).

The performance results demonstrate this:
- Row-major vs column-major: 1.9-3.6x speedup from better spatial locality
- Compiler -O3: 1.5-9.4x speedup from register use and instruction-level optimizations
- Memory alignment: ~1% difference as modern CPUs handle unaligned access efficiently
- Cache blocking: 2.3-2.6x speedup from better temporal locality (see Section 6)

The optimization hierarchy by importance is: (1) algorithm design for cache locality, (2) compiler optimization flags, (3) data structure layout, (4) fine-tuning like alignment.

### 6. Implementing Cache Blocking

We implemented cache blocking (also called tiling) for matrix multiplication, the most important optimization technique for dense linear algebra.

#### Why the Naive Implementation Needs Blocking

The naive implementation has poor cache behavior because:
1. It extracts columns from row-major matrix B using strided access (jumping `colsB` elements)
2. It allocates temporary arrays for each column of B
3. It does not reuse data — each element of A and B is loaded from memory multiple times

Cache blocking fixes this by dividing the matrices into smaller tiles that fit in cache, then computing the result tile by tile.

#### Implementation Details

We chose a block size of 32×32 elements, which is 8KB per block. Three such blocks (two inputs, one output) take 24KB, fitting comfortably in a typical 32KB L1 cache. We also changed the loop order from i-j-k to i-k-j, which is crucial for performance:

```cpp
for (int ii = 0; ii < rowsA; ii += BLOCK_SIZE) {
    for (int kk = 0; kk < colsA; kk += BLOCK_SIZE) {
        for (int jj = 0; jj < colsB; jj += BLOCK_SIZE) {
            for (int i = ii; i < i_max; ++i) {
                for (int k = kk; k < k_max; ++k) {
                    double a_val = matrixA[i * colsA + k];  // Load once
                    for (int j = jj; j < j_max; ++j) {
                        result[i * colsB + j] += a_val * matrixB[k * colsB + j];
                    }
                }
            }
        }
    }
}
```

The key optimization is loading `A[i,k]` once in the middle loop and reusing it across an entire row of B. This provides an innermost loop that the compiler can effectively vectorize.

#### Performance Results

| Matrix Size | Naive (ms) | Blocked (ms) | Speedup |
|-------------|------------|--------------|---------|
| 128×128     | 0.69      | 0.30        | 2.30x   |
| 256×256     | 6.69      | 2.76        | 2.42x   |
| 512×512     | 70.22     | 27.13       | 2.59x   |
| 1024×1024   | 605.62    | 249.61      | 2.43x   |

Cache blocking provides a consistent 2.3-2.6x speedup across all sizes. The speedup is relatively constant because the blocking strategy works at all scales — each 32×32 tile benefits from being cache-resident regardless of the overall matrix size.

#### Analysis

Without blocking, there are O(N³) cache misses because each element gets loaded from memory multiple times. With blocking, this is reduced to O(N³/B) cache misses where B is the block size, because elements are reused while they remain in cache.

The i-k-j loop order is important because it enables sequential access to B. In each iteration of the innermost j-loop, consecutive elements of B are accessed, which the hardware prefetcher can predict. The compiler can also vectorize this loop effectively.

Comparison to other optimizations:
- vs Naive: 2.4x faster (cache blocking)
- vs Transposed B: 2.2x faster 
- vs -O3 compiler optimization: Orthogonal — blocking works on top of -O3

#### Further Optimizations

Additional optimizations are possible: adaptive block sizes based on cache detection, explicit SIMD instructions, multi-threading with OpenMP. Combined, these optimizations (blocking 2.4x × SIMD 4x × threading 8x) could theoretically provide 77x speedup, approaching optimized BLAS libraries. For this project, the 2.4x from blocking demonstrates the core principle.

### Summary

High-performance computing depends on understanding the memory hierarchy. The CPU is fast, but memory is slow, so performance depends on keeping frequently accessed data in the fastest cache level.

Key findings:
1. **Memory access patterns dominate** — row-major vs column-major provides 3.6x speedup
2. **Algorithm design matters most** — cache blocking provides 2.4x speedup on top of other optimizations
3. **Compiler optimizations are essential** — -O3 provides up to 9.4x speedup
4. **Micro-optimizations have diminishing returns** — alignment provides ~1% benefit

The optimization hierarchy is: first design cache-friendly algorithms, then use compiler optimizations, then optimize data layout, and only at the end consider fine-tuning like alignment.

Production code should use libraries like OpenBLAS or Intel MKL that implement these optimizations plus many more (multi-level blocking, SIMD, multi-threading, architecture-specific tuning). Implementing blocking demonstrates why those libraries are significantly faster than naive implementations.
