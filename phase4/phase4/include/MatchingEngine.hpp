#pragma once

#include "MarketData.hpp"
#include "Order.hpp"
#include "OrderBook.hpp"
#include "OrderManager.hpp"
#include "Timer.hpp"

#include <algorithm>
#include <vector>

template <typename PriceType, typename OrderIdType>
struct MatchedTrade {
    OrderIdType buy_order_id;
    OrderIdType sell_order_id;
    PriceType price;
    int quantity;
    std::chrono::high_resolution_clock::time_point tick_timestamp;
};

template <typename PriceType, typename OrderIdType>
class MatchingEngine {
public:
    using OrderType = Order<PriceType, OrderIdType>;
    using TradeType = MatchedTrade<PriceType, OrderIdType>;

    std::vector<TradeType> matchOrder(
        const MarketData& tick,
        OrderType incoming,
        OrderBook<PriceType, OrderIdType>& book,
        OrderManager<OrderIdType>& order_manager) {
        std::vector<TradeType> trades;
        Timer timer;
        timer.start();

        order_manager.registerNew(incoming.id);
        while (incoming.quantity > 0 && book.hasOppositeLiquidity(incoming.is_buy)) {
            OrderType* opposite = book.bestOpposite(incoming.is_buy);
            if (opposite == nullptr || !book.crossesSpread(incoming, *opposite)) {
                break;
            }

            const int qty = std::min(incoming.quantity, opposite->quantity);
            incoming.quantity -= qty;

            TradeType trade{
                incoming.is_buy ? incoming.id : opposite->id,
                incoming.is_buy ? opposite->id : incoming.id,
                opposite->price,
                qty,
                tick.timestamp};
            trades.push_back(trade);

            book.reduceOrRemove(opposite->id, qty);
            if (incoming.quantity == 0) {
                order_manager.markFilled(incoming.id);
            } else {
                order_manager.markPartial(incoming.id);
            }
        }

        if (incoming.quantity > 0) {
            book.addOrder(std::move(incoming));
        }

        latencies_ns_.push_back(timer.stop());
        return trades;
    }

    const std::vector<long long>& latencies() const {
        return latencies_ns_;
    }

private:
    std::vector<long long> latencies_ns_;
};
