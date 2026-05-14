#include <iostream>
#include <mutex>
#include <thread>

std::mutex mutex_a, mutex_b;

void thread_a() {

  std::lock_guard<std::mutex> lock_1(mutex_a);
  std::cout << "Thread A obtained Mutex A";

  std::lock_guard<std::mutex> lock_2(mutex_b);
  std::cout << "Thread A obtained Mutex B";
}

void thread_b() {

  std::lock_guard<std::mutex> lock_1(mutex_a);
  std::cout << "Thread B obtained Mutex A";

  std::lock_guard<std::mutex> lock_2(mutex_b);
  std::cout << "Thread B obtained Mutex B";
}

int main() {
  std::thread a(thread_a);
  std::thread b(thread_b);

  a.join();
  b.join();

  std::cout << "Done\n";
}
