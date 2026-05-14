#include <chrono>
#include <iostream>
#include <thread>

#include <condition_variable>

bool ready = false;

std::mutex m;
std::condition_variable cv;

void producer() {
  // std::this_thread::sleep_for(std::chrono::seconds(2));
  // ready = true;

  // std::cout << "Ready for work!\n";

  {
    std::lock_guard lock(m);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ready = true;
    std::cout << "Ready for work!\n";
  }
  cv.notify_one();
}

void consumer() {
  // while (!ready)
  //   std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // std::cout << "Doing work!\n";

  std::unique_lock lock(m);
  cv.wait(lock, [] { return ready; });
  std::cout << "Doing work!\n";
}

int main() {
  std::thread p(producer);
  std::thread c(consumer);

  p.join();
  c.join();
}
