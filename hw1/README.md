# Homework 1: Fast Matrix Summation

The baseline run time of the unoptimized functions was 58 ms. My goal was to halve the run time, getting it to be under 29 ms. I implemented the following changes:

List of changes:
- Replace `sum += matrix[i][j];` with `sum += matrix[i][j`, removing function calls from the stack and directly accessing the matrix. By removing function calls, the compiler doesn't need to use as many `jmp` instructions nor recreate the stack for function scoping, making the code more efficient.
  - New runtime: 26 ms
- Get the value of elements in the vector using pointers from each vector using `std::vector.data()`. Direct access using pointers is faster than going through the API exposed by `std::vector`. I tried doing point arithmetic with one pointer, but the metadata used for `std::vector` introduced garbage values to the sum, so I instead used direct pointer access per row.
  - New runtime: 9 ms

Example output:

```
hw1 % ./a.out
Basic Sum: 13747
Basic Time: 61 milliseconds
Optimized Sum: 13747
Optimized Time: 9 milliseconds
```

The optimized function is correct, as it provides the same output as the basic function, and the runtime is reduced by 75%.
