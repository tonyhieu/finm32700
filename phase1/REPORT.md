# Phase 1 Report: High-Performance Linear Algebra Kernels

## Part 2: Performance Analysis and Optimization

### 1. Benchmarking Framework

We developed a comprehensive benchmarking framework using `std::chrono` to measure execution times with microsecond precision. The framework includes:

- **Multiple runs**: Each test performs 5-20 iterations
- **Statistical analysis**: Calculates mean, standard deviation, min, and max execution times
- **Warm-up**: Initial runs to ensure cache is populated
- **Variance tracking**: Standard deviation to assess measurement stability

### 2. Cache Locality Analysis

#### 2.1 Matrix-Vector Multiplication: Row-Major vs Column-Major

**Test Setup**: Square matrices of varying sizes (100×100 to 2000×2000)

**Results**:

| Size | Row-Major (ms) | Column-Major (ms) | Speedup |
|------|----------------|-------------------|---------|
| 100  | 0.0061        | 0.0220           | 3.63x   |
| 500  | 0.2033        | 0.4432           | 2.18x   |
| 1000 | 0.6952        | 1.3624           | 1.96x   |
| 2000 | 2.1883        | 4.1821           | 1.91x   |

**Analysis**:

Row-major matrix-vector multiplication consistently outperforms column-major by **1.9-3.6x**. This is because:

1. **Spatial Locality**: In row-major order, matrix elements in the same row are stored contiguously in memory. When accessing `matrix[r*cols + c]` and incrementing `c`, we're accessing adjacent memory locations.

2. **Cache Line Utilization**: Modern CPUs fetch entire cache lines (typically 64 bytes = 8 doubles). Row-major access loads a cache line and uses all 8 values before moving to the next line. Column-major access loads a cache line but only uses one value, leading to poor cache utilization.

3. **Prefetcher Efficiency**: Hardware prefetchers detect sequential access patterns in row-major, bringing the next cache line before it's needed. Column-major's strided access pattern (jumping `rows` elements) is harder for prefetchers to predict.

#### 2.2 Matrix-Matrix Multiplication: Naive vs Transposed B

**Test Setup**: Square matrices (64×64 to 512×512)

**Results**:

| Size | Naive (ms) | Transposed B (ms) | Speedup |
|------|------------|-------------------|---------|
| 64   | 0.1925    | 0.1584           | 1.22x   |
| 128  | 1.1855    | 2.2491           | 0.53x   |
| 256  | 9.2796    | 5.5776           | 1.66x   |
| 512  | 62.5985   | 53.5158          | 1.17x   |

**Analysis**:

Results show variable performance depending on matrix size:

- **Small matrices (64×64)**: Transposed B is 1.22x faster. Both matrices fit in L1 cache, transposed approach benefits from better access patterns.

- **Medium matrices (128×128)**: Naive is actually 1.9x faster (0.53x speedup). This anomaly likely occurs because our naive implementation uses `multiply_mv_row_major` which extracts columns efficiently.

- **Large matrices (256×256, 512×512)**: Transposed B wins by 1.17-1.66x. As matrices exceed cache sizes, the transposed approach's row-row dot product pattern provides better cache locality.

**Memory Access Pattern Comparison**:

- **Naive (A×B)**: Accesses A row-wise (good) but B column-wise (poor). Each element of B requires jumping `colsB` positions in memory.
  
- **Transposed (A×B^T)**: Both A and B^T accessed row-wise (good). Each inner loop computes a dot product of two rows with sequential memory access.

### 3. Memory Alignment

**Test Setup**: Matrix-matrix multiplication with unaligned vs 64-byte aligned allocations

**Results**:

| Size | Unaligned (ms) | 64-byte Aligned (ms) | Speedup |
|------|----------------|----------------------|---------|
| 256  | 5.2435        | 5.1680              | 1.01x   |
| 512  | 51.6713       | 51.2517             | 1.01x   |
| 1024 | 490.6303      | 500.3716            | 0.98x   |

**Analysis**:

Memory alignment showed **minimal impact** (±1-2%) on performance:

1. **Modern CPUs Handle Misalignment**: Current x86-64 processors have hardware support for unaligned loads/stores with minimal penalty

2. **Compiler Optimization**: At `-O3`, the compiler may already be optimizing memory access patterns

3. **Cache Line Boundaries**: With 64-byte alignment, each row starts at a cache line boundary, potentially preventing false sharing in multi-threaded scenarios (not tested here)

4. **SIMD Potential**: While our code doesn't use SIMD instructions, aligned memory is required for aligned SIMD loads (movaps vs movups), which could show 2-3x speedup

**Recommendation**: For single-threaded code without explicit SIMD, alignment provides marginal benefits. For SIMD-optimized or multi-threaded code, 64-byte alignment becomes more important.

### 4. Compiler Optimization Impact

**Test Setup**: Comparing performance of our implemented functions compiled with -O0 (no optimization) vs -O3 (aggressive optimization)

#### 4.1 Matrix-Vector Multiplication

**Results at -O0**:

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

**Speedup from -O3 optimization**:

| Size | Row-Major | Col-Major |
|------|-----------|-----------|
| 100  | 4.20x    | 2.37x    |
| 500  | 2.67x    | 1.64x    |
| 1000 | 2.37x    | 1.46x    |
| 2000 | 1.97x    | 1.91x    |

#### 4.2 Matrix-Matrix Multiplication

**Results at -O0**:

| Size | Naive (ms) | Transposed B (ms) |
|------|------------|-------------------|
| 64   | 0.2989    | 0.3151           |
| 128  | 2.2706    | 4.8892           |
| 256  | 17.6031   | 20.3496          |
| 512  | 140.0751  | 162.3422         |

**Results at -O3**:

| Size | Naive (ms) | Transposed B (ms) |
|------|------------|-------------------|
| 64   | 0.0530    | 0.0498           |
| 128  | 0.5819    | 0.5226           |
| 256  | 6.4556    | 5.5141           |
| 512  | 58.3391   | 55.7102          |

**Speedup from -O3 optimization**:

| Size | Naive | Transposed B |
|------|-------|--------------|
| 64   | 5.64x | 6.33x       |
| 128  | 3.90x | 9.36x       |
| 256  | 2.73x | 3.69x       |
| 512  | 2.40x | 2.91x       |

**Analysis**:

1. **Massive Performance Improvement**: -O3 provides **1.5-9.4x speedup** across all functions and sizes
   - Smallest matrices benefit most (up to 9.4x for 128×128 transposed B)
   - Larger matrices still see 2-2.5x improvement
   - Row-major matrix-vector shows most consistent gains (2-4.2x)

2. **What -O3 Does**:
   - **Loop unrolling**: Reduces loop overhead by processing multiple iterations at once
   - **Instruction scheduling**: Reorders operations to minimize CPU stalls
   - **Register allocation**: Keeps frequently used values in CPU registers instead of memory
   - **Dead code elimination**: Removes unnecessary computations
   - **Auto-vectorization**: May use SIMD instructions where possible
   - **Inlining**: Eliminates function call overhead automatically

3. **Function-Specific Observations**:
   - **Matrix-Vector**: Row-major gains more from optimization (4.2x vs 2.4x for small sizes) because the sequential access pattern enables better prefetching and vectorization
   - **Matrix-Matrix**: Transposed B shows dramatic improvement at size 128 (9.36x), suggesting the compiler effectively optimizes the row-row dot product pattern
   - **Larger matrices**: As size increases, memory bandwidth becomes the bottleneck, reducing optimization impact

4. **Cache Effects Preserved**: Even with -O3, cache-friendly access patterns matter:
   - Row-major still outperforms column-major at -O3
   - Transposed B still shows benefits for larger matrices
   - Optimization amplifies good algorithm design, doesn't replace it

**Recommendation**: 
- **Always compile with -O3** (or at least -O2) for production code
- Optimization provides "free" performance without code changes
- Focus on algorithm design first (cache locality, access patterns)
- Let the compiler handle low-level optimizations (inlining, loop unrolling)

### 5. Key Findings and Cache Behavior Summary

#### CPU Cache Hierarchy (Typical Modern CPU)

- **L1 Cache**: 32-64 KB per core, ~4 cycles latency, separate for data/instructions
- **L2 Cache**: 256-512 KB per core, ~12 cycles latency
- **L3 Cache**: 8-32 MB shared, ~40 cycles latency
- **Main Memory**: GB scale, ~200 cycles latency

#### Temporal vs Spatial Locality

**Temporal Locality**: Accessing the same data multiple times
- Exploited by: Blocking/tiling algorithms that reuse data while in cache
- Example: In matrix multiplication, computing multiple elements that use the same row of A

**Spatial Locality**: Accessing nearby data
- Exploited by: Sequential memory access patterns
- Example: Row-major access of matrices

#### Optimizations Applied

1. **Row-Major Access**: 1.9-3.6x faster than column-major for matrix-vector
2. **Transposed B**: 1.17-1.66x faster for large matrix-matrix multiplication
3. **Compiler -O3**: 1.5-9.4x faster than -O0 (varies by function and size)
4. **Memory Alignment**: Minimal impact (~1%) without SIMD

### 6. Future Optimization Opportunities

1. **Blocking/Tiling**: Divide matrices into cache-sized blocks (not yet implemented)
2. **SIMD Vectorization**: Use AVX2/AVX-512 for 4-8x parallelism
3. **Loop Unrolling**: Reduce loop overhead (compiler may do this at -O3)
4. **Prefetching**: Manual prefetch instructions for predictable access patterns
5. **Multi-threading**: Parallelize across rows or tiles using OpenMP

### Conclusion

Cache locality dominates performance in linear algebra operations. Our analysis shows:

- **Memory access patterns matter more than alignment** (3.6x vs 1% speedup)
- **Compiler optimizations are essential** (28-40% improvement)
- **Row-major access patterns exploit spatial locality** effectively
- **Algorithm choice** (transposed B) can improve cache utilization for large matrices

The hierarchy of optimization priorities is:
1. Algorithm design (cache-friendly access patterns)
2. Compiler optimization flags
3. Data structure layout (row/column major)
4. Fine-tuning (alignment, manual inlining)
