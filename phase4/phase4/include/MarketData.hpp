#pragma once

#include <chrono>
#include <string>

struct alignas(64) MarketData {
  std::string symbol;
  double bid_price{0.0};
  double ask_price{0.0};
  std::chrono::high_resolution_clock::time_point timestamp{};
};

class MarketDataFeed {
public:
  MarketDataFeed(std::string symbol, double base_price, double tick_size);
  MarketData nextTick(int tick_index);

private:
  std::string symbol_;
  double base_price_{0.0};
  double tick_size_{0.01};
};
