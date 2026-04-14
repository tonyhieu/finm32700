#include "feed_parser.h"
#include "market_snapshot.h"
#include "order_manager.h"

#include <deque>
#include <iomanip>
#include <iostream>

namespace {

void print_best(const MarketSnapshot &snapshot) {
  const PriceLevel *bid = snapshot.get_best_bid();
  const PriceLevel *ask = snapshot.get_best_ask();

  std::cout << std::fixed << std::setprecision(2);
  if (bid) {
    std::cout << "[Market] Best Bid: " << bid->price << " x " << bid->quantity
              << "\n";
  } else {
    std::cout << "[Market] Best Bid: none\n";
  }

  if (ask) {
    std::cout << "[Market] Best Ask: " << ask->price << " x " << ask->quantity
              << "\n";
  } else {
    std::cout << "[Market] Best Ask: none\n";
  }
}

} // namespace

int main() {
  MarketSnapshot snapshot;
  OrderManager order_manager;

  const auto feed = load_feed("sample_feed.txt");
  int total_orders_placed = 0;
  constexpr std::size_t kMidWindow = 4;
  std::deque<double> mid_prices;
  std::deque<double> spreads;
  double mid_sum = 0.0;
  double spread_sum = 0.0;

  for (const auto &event : feed) {
    if (event.type == FeedType::BID) {
      snapshot.update_bid(event.price, event.quantity);
    } else if (event.type == FeedType::ASK) {
      snapshot.update_ask(event.price, event.quantity);
    } else if (event.type == FeedType::EXECUTION) {
      order_manager.handle_fill(event.order_id, event.quantity);
    }

    print_best(snapshot);

    const PriceLevel *best_bid = snapshot.get_best_bid();
    const PriceLevel *best_ask = snapshot.get_best_ask();
    if (!best_bid || !best_ask) {
      continue;
    }

    const double mid_price = (best_bid->price + best_ask->price) / 2.0;
    const double spread = best_ask->price - best_bid->price;

    mid_prices.push_back(mid_price);
    mid_sum += mid_price;
    spreads.push_back(spread);
    spread_sum += spread;
    if (mid_prices.size() > kMidWindow) {
      mid_sum -= mid_prices.front();
      mid_prices.pop_front();
      spread_sum -= spreads.front();
      spreads.pop_front();
    }
    if (mid_prices.size() < kMidWindow) {
      continue;
    }

    const double avg_mid_price =
        mid_sum / static_cast<double>(mid_prices.size());
    const double avg_spread = spread_sum / static_cast<double>(spreads.size());
    const double dynamic_bid_threshold = avg_mid_price - (0.50 * avg_spread);
    const bool sell_signal = best_bid->price >= dynamic_bid_threshold &&
                             mid_price >= avg_mid_price &&
                             spread <= (avg_spread + 0.02);

    if (sell_signal && order_manager.active_order_count() == 0 &&
        total_orders_placed < 2) {
      const int qty = (total_orders_placed == 0) ? 50 : 100;
      std::cout << "[Strategy] Signal: mid " << mid_price << " vs avg "
                << avg_mid_price << ", threshold " << dynamic_bid_threshold
                << ", spread " << spread << " avg_spread " << avg_spread
                << "\n";
      order_manager.place_order(Side::Sell, best_ask->price, qty);
      ++total_orders_placed;
    }
  }

  order_manager.print_active_orders();
  return 0;
}
