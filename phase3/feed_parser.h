#pragma once

#include <string>
#include <vector>

enum class FeedType { BID, ASK, EXECUTION };

struct FeedEvent {
    FeedType type;
    double price = 0.0;
    int quantity = 0;
    int order_id = -1;
};

std::vector<FeedEvent> load_feed(const std::string& filename);
