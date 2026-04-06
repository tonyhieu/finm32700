# Homework 2 Write-up

As seen in `output.txt`, the signal that triggered the most orders was by far signal 2. Some optimizations that could be made are:
- The `getAvg` function does not have to sum all ticks every time; it can keep a running sum of tick prices to find the average in O(1) time.
  - A running sum would also remove the need for `updateHistory` in most cases; instead of erasing from the beginning (which is O(n)), we can subtract the element at `hist.size() - 10` and add the new element. 
- The `if` conditions starting on line 55 can be condensed into an `if-else` chain, as there is repetitive work being done if `buy = true` is being set multiple times.

With 10x data and additional signals, we would see a significant slowdown. Using constants instead of collections, as mentioned above, would help the code run faster in the face of more data.
