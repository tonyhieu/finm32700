// #include <cfloat>
// #include <iostream>
// #include <memory>
// #include <string>
//
// class Trade {
// public:
//   Trade(std::string s, int q) : symbol(s), quantity(q) {}
//   std::string symbol;
//   int quantity;
// };
//
// class MarketData {
// public:
//   double price(const std::string &symbol) const {
//     return 123.45; // dummy price
//   }
// };
//
// class Observer {
// public:
//   Observer(std::weak_ptr<MarketData> md) : market(md) {}
//
//   void check() {
//     if (auto locked = market.lock()) {
//       std::cout << "MarketData available\n";
//     } else {
//       std::cout << "MarketData expired\n";
//     }
//   }
//
// private:
//   std::weak_ptr<MarketData> market;
// };
//
// class Double {
// public:
//   Double(std::shared_ptr<MarketData> md) : data_(md) {}
//
//   double perform_op() { return data_->price("AAPL") * 2; }
//
// private:
//   std::shared_ptr<MarketData> data_;
// };
//
// class Triple {
// public:
//   Triple(std::shared_ptr<MarketData> md) : data_(md) {}
//
//   double perform_op() { return data_->price("AAPL") * 3; }
//
// private:
//   std::shared_ptr<MarketData> data_;
// };
//
// int main() {
//   // TODO: Create a unique_ptr<Trade> for a single trade
//
//   auto trade = std::make_unique<Trade>("AAPL", 10);
//
//   // TODO: Create a shared_ptr<MarketData>
//
//   auto data = std::make_shared<MarketData>();
//
//   // TODO: Pass shared_ptr to two different pricing operations
//
//   Triple *t = new Triple(data);
//   Double *d = new Double(data);
//
//   // TODO: Create an Observer using weak_ptr
//
//   Observer o(data);
//
//   // TODO: Demonstrate observer detecting expiration
//
//   data = nullptr;
//   delete t;
//   delete d;
//
//   o.check();
//
//   return 0;
// }
//
//

#include <atomic>
#include <iostream>
#include <thread>

std::atomic_int balance;

int main() {
  std::thread inc([&]() {
    for (int i = 0; i < 10000000; ++i)
      ++balance;
  });

  std::thread dec([&]() {
    for (int i = 0; i < 10000000; ++i)
      --balance;
  });

  inc.join();
  dec.join();

  std::cout << "Final balance: " << balance << '\n';
}
