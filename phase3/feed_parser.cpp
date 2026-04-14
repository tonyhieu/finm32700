#include "feed_parser.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::vector<FeedEvent> load_feed(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<FeedEvent> events;

    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << filename << "\n";
        return events;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "BID") {
            FeedEvent event;
            event.type = FeedType::BID;
            if (iss >> event.price >> event.quantity) {
                events.push_back(event);
            }
        } else if (type == "ASK") {
            FeedEvent event;
            event.type = FeedType::ASK;
            if (iss >> event.price >> event.quantity) {
                events.push_back(event);
            }
        } else if (type == "EXECUTION") {
            FeedEvent event;
            event.type = FeedType::EXECUTION;
            if (iss >> event.order_id >> event.quantity) {
                events.push_back(event);
            }
        } else {
            std::cerr << "Unknown event type: " << line << "\n";
        }
    }

    return events;
}
