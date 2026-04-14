# Homework 3

## Part 1

1. A warning is generated during compile-time, and an error is thrown: `zsh: trace trap ./a.out`.
2. Nothing happens, but repeatedly forgetting to call delete will result in memory leakage. This is because the code is technically correct and compiles correctly, but the memory that was allocated never gets freed, leading to long-term problems.
3. Calling delete on the same pointer returns `zsh: trace trap  ./a.out`. This is because we are trying to delete the same pointer twice, causing an error.

## Part 3

1. We delete the copy constructor because `TradeHandle` is supposed to only allow for unique pointers; copying a pointer would break the guarantee of uniqueness.
2. Move semantics are allowed because the rvalue will not maintain a pointer to the trade. The pointer from the rvalue will be given directly to the `TradeHandle` instance, maintaining the uniqueness guarantee.
3. If you don't define a destructor, the pointer never gets freed, leading to memory leaks.

## Part 5

1. The 3 biggest risks of manual memory management are memory leaks, use-after-free errors, and double frees. These lead to errors that are difficult to diagnose and fix, and they can cause systems to fail while running critical code.
2. Some real-world problems that could occur in HFT systems due to memory leaks or fragmentation are crashes during trade execution, high latency due to memory leakage, and data loss.
3. RAII reduces the risk of bugs by guaranteeing memory safety for users. Instead of having to manage memory, users only have to interface with surface-level abstractions (i.e. initializing an object without worrying about deleting the memory it is using). 
4. While automatic memory management reduces the cognitive load on programmers, the fact that the memory management is abstracted away means that there could be over-generalizations or slow downs in the implementation that programmers don't know about due to never "looking under the hood". In performance-critical code, these inefficiencies can add up and cause major harm to the systems that rely on it.
