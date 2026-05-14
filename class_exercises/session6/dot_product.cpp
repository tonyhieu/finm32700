#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

std::atomic<int> sum = 0;

void dot_product(const std::vector<float> &a, const std::vector<float> &b,
                 int start, int end) {
  int sum = 0;
  for (int i = start; i < end; ++i)
    sum += a[i] * b[i];
}

void thread_dot_product(int threads, std::vector<float> a,
                        std::vector<float> b) {
  std::vector<std::thread> thread_vec;
  auto size = a.size();
  int thread_size = std::ceil(size / threads);

  for (int i = 0; i < threads; ++i) {
    std::thread t(dot_product, a, b, i * thread_size,
                  std::min((i + 1) * thread_size, (int)size));
    thread_vec.push_back(std::move(t));
  }

  for (auto &t : thread_vec)
    t.join();
}

int main() {
  std::vector<float> results;
  std::vector<int> threads{1, 2, 4, 6, 8, 10, 12, 16, 32};

  std::vector<float> a;
  std::vector<float> b;

  std::srand(42);
  for (int i = 0; i < 10000000; ++i) {
    a.push_back(std::rand() % 1000);
    b.push_back(std::rand() % 1000);
  }

  for (auto thread : threads) {
    auto start = std::chrono::steady_clock::now();
    thread_dot_product(thread, a, b);
    auto end = std::chrono::steady_clock::now();
    sum = 0;
    std::chrono::duration<float> elapsed = end - start;
    results.push_back(elapsed.count());
  }

  for (auto &r : results)
    std::cout << r << '\n';
}
