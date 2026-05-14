#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include <sys/wait.h>
#include <unistd.h>

ssize_t write_full(int fd, const void *buffer, size_t count) {
  const char *data = static_cast<const char *>(buffer);
  size_t total = 0;

  while (total < count) {
    ssize_t written = write(fd, data + total, count - total);
    if (written <= 0)
      return written;
    total += written;
  }

  return static_cast<ssize_t>(total);
}

ssize_t read_full(int fd, void *buffer, size_t count) {
  char *data = static_cast<char *>(buffer);
  size_t total = 0;

  while (total < count) {
    ssize_t n = read(fd, data + total, count - total);
    if (n <= 0)
      return n;
    total += n;
  }

  return static_cast<ssize_t>(total);
}

double run_thread_pipeline(int n_items) {
  std::queue<int> q;
  std::mutex m;
  std::condition_variable cv;
  bool done = false;
  std::uint64_t consumed_sum = 0;

  auto start = std::chrono::steady_clock::now();

  std::thread producer([&]() {
    for (int i = 0; i < n_items; ++i) {
      {
        std::lock_guard<std::mutex> lock(m);
        q.push(i);
      }
      cv.notify_one();
    }

    {
      std::lock_guard<std::mutex> lock(m);
      done = true;
    }
    cv.notify_one();
  });

  std::thread consumer([&]() {
    while (true) {
      std::unique_lock<std::mutex> lock(m);
      cv.wait(lock, [&] { return !q.empty() || done; });

      if (q.empty() && done)
        break;

      int value = q.front();
      q.pop();
      lock.unlock();

      consumed_sum += value;
    }
  });

  producer.join();
  consumer.join();

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  std::cout << "Thread sum: " << consumed_sum << '\n';
  return elapsed.count();
}

double run_process_pipeline(int n_items) {
  int pipe_fd[2];
  if (pipe(pipe_fd) != 0) {
    std::cerr << "pipe failed\n";
    return -1.0;
  }

  auto start = std::chrono::steady_clock::now();
  pid_t pid = fork();

  if (pid < 0) {
    std::cerr << "fork failed\n";
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    return -1.0;
  }

  if (pid == 0) {
    close(pipe_fd[1]);
    std::uint64_t consumed_sum = 0;

    while (true) {
      std::int32_t value = 0;
      ssize_t n = read_full(pipe_fd[0], &value, sizeof(value));

      if (n <= 0)
        break;

      if (value == -1)
        break;

      consumed_sum += static_cast<std::uint32_t>(value);
    }

    close(pipe_fd[0]);
    std::cout << "Process sum: " << consumed_sum << '\n';
    _exit(0);
  }

  close(pipe_fd[0]);

  for (int i = 0; i < n_items; ++i) {
    std::int32_t value = i;
    if (write_full(pipe_fd[1], &value, sizeof(value)) <= 0) {
      std::cerr << "write failed\n";
      break;
    }
  }

  std::int32_t sentinel = -1;
  write_full(pipe_fd[1], &sentinel, sizeof(sentinel));
  close(pipe_fd[1]);

  waitpid(pid, nullptr, 0);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  return elapsed.count();
}

int main() {
  int n_items = 200000;

  double thread_time = run_thread_pipeline(n_items);
  double process_time = run_process_pipeline(n_items);

  if (thread_time > 0) {
    std::cout << "Thread time: " << thread_time << " s\n";
    std::cout << "Thread throughput: " << n_items / thread_time << " items/s\n";
    std::cout << "Thread latency: " << (thread_time / n_items) * 1e6
              << " us/item\n";
  }

  if (process_time > 0) {
    std::cout << "Process time: " << process_time << " s\n";
    std::cout << "Process throughput: " << n_items / process_time
              << " items/s\n";
    std::cout << "Process latency: " << (process_time / n_items) * 1e6
              << " us/item\n";
  }
}
