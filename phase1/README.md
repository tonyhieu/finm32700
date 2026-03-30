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

