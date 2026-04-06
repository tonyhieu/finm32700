# Phase 1 - High-Performance Linear Algebra Kernels

## Build Instructions

To compile the benchmark suite:
```bash
make
```

To run the benchmarks:
```bash
./benchmark
```

To compile and run tests:
```bash
g++ -std=c++11 -Wall -Wextra -O3 -o tests tests.cpp matrix_operations.cpp
./tests
```

To clean build artifacts:
```bash
make clean
```

## Discussion Questions

1. A pointer is a data type that stores the address of a variable separately from the variable itself. A reference is an alias that stores the address of a variable. This distinction means that references must be initialized, cannot be changed to refer to a different object, and cannot be null, whereas pointers do not follow these rules. References are best used for function parameters and return types where the object is guaranteed to exist whereas pointers are best used for situations where nullability or pointer arithmetic is required. In the context of implementing numerical algorithms, references are good for passing in abstract containers to ensure that they aren't copied (i.e. pass by reference) and pointers are good for implementing abstract containers.

2. Row-major and column-major order have different performances due to cache locality. Specifically, matrices stored in row-major order enjoy speedups compared to ones stored in column-major order because contiguous rows are accessed from memory and are stored in the cache while contiguous columns are stored in the cache but only one value per column is used, leading to poor cache utilization. This can be seen in both `trace.png` as well as the benchmarking test that compares row- vs. column-major order for matrix-vector operations.

3. When data is accessed from memory, it is stored in caches so that the CPU does not need to re-fetch it. The closest cache to the CPU is the L1 cache, and it has the fastest access times. L2 is the second closest, and L3 is the third closest. Data is removed from closer caches and placed in further caches (or physical memory) based on a cache eviction policy set by the operating system. Temporal locality refers to the fact that data that was accessed more recently is faster to access than data that was accessed long ago. Spatial locality refers to the fact that pieces of data that are close together are faster to access than pieces of data that are further from each other. We exploited these concepts in our optimizations by using blocking, which allows one piece of a matrix to be stored in the cache at a time, as well as row-major order for better spatial locality.

4. Memory alignment refers to the concept of creating data structures that don't waste space due to poorly aligned members. For example, if an `int` takes 4 bytes and a `char` takes 1 byte, and addresses are stored by a factor of 4 bytes, then a data structure that uses two `int`s and a `char` has better memory alignment if its initialization order is `int`, `int`, `char` (9 bytes) versus `int`, `char`, `int` (12 bytes, as the `int` needs 3 bytes of padding). Alignment did not result in a significant performance boost. Some reasons for this are that modern compilers optimize away misalignment issues and that modern CPUs can handle misalignment.

5. Compiler optimizations reduce the number of instructions used, leading to less work for the computer and faster programs. Inlining is an optimization that removes the need for jump instructions, as inlined functions are treated as being inside of their parent function and don't need to be jumped to. Compiler optimization made our functions run much faster, with ranging from 3x to 9x speedups. Aggressive optimizations can lead to unwanted behavior and lead to bugs. Additionally, optimizations make programs longer to compile.

6. Profiling confirmed our belief that column-major operations are less efficient than row-major ones and that there isn't much of a difference between the naive and transposed implementations of matrix multiplication. Because we did not use many function calls (everything was inlined), the profiler did not reveal many inefficiencies in our implementation. Instead, we researched different optimization techniques to implement instead.

7. This assignment was completed by one person.
