#include <iostream>
#include <string>

struct Trade {
  std::string symbol;
  double price;

  Trade(const std::string &sym, double p) : symbol(sym), price(p) {}

  ~Trade() { std::cout << "Trade destroyed: " << symbol << '\n'; }
};

void part1() {
  Trade *single = new Trade("AAPL", 1);
  Trade **arr = new Trade *[5];
  for (int i = 0; i < 5; ++i)
    arr[i] = new Trade("AAPL", i + 10);

  single->symbol = "GOOG";
  for (int i = 0; i < 5; ++i)
    arr[i]->symbol = "GOOG";

  delete single;
  delete single;
  for (int i = 0; i < 5; ++i)
    delete arr[i];

  delete[] arr;
}

void part2() {
  Trade *t1 = new Trade("AAPL", 150.0);
  Trade *t2 = new Trade("GOOG", 2800.0);
  delete t1;
  // delete t1; Commented out this line because you cannot delete the same
  // pointer twice.

  Trade *t3 = new Trade("MSFT", 300.0);
  delete t3; // Added this line. If you want to re-allocate to t3, then you need
             // to delete the initial allocation. If this doesn't happen, then
             // memory leaks occur.
  t3 = new Trade("TSLA", 750.0);

  Trade *trades =
      new Trade[3]{{"NVDA", 900.0}, {"AMZN", 3200.0}, {"META", 250.0}};
  delete[] trades; // Changed delete to delete[]. Since we used the new[]
                   // operator to instantiate trades, we must use the delete[]
                   // operator to delete it.
}

class TradeHandle {
  Trade *ptr;

public:
  TradeHandle(Trade *p) : ptr(p) {}

  ~TradeHandle() { delete ptr; }

  TradeHandle(const TradeHandle &) = delete;

  TradeHandle &operator=(const TradeHandle &) = delete;

  TradeHandle(TradeHandle &&other) {
    ptr = other.ptr;
    other.ptr = nullptr;
  }

  TradeHandle &operator=(TradeHandle &&other) {
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
  }

  Trade *operator->() { return ptr; }

  Trade &operator*() { return *ptr; }
};

void trade_handle_example() {
  auto trade1 = TradeHandle(new Trade("MSFT", 10));
  auto trade2 = TradeHandle(new Trade("GOOG", 100));

  std::cout << trade1->symbol << '\n';
  std::cout << (*trade2).price << '\n';
}

int main() {
  trade_handle_example();

  return 0;
}
