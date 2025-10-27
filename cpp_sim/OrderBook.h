#pragma once

#include <map>
#include <deque>
#include "Order.h"

class OrderBook {
public:
    using Queue = std::deque<Order>;
    using Bids = std::map<double, Queue, std::greater<>>;
    using Asks = std::map<double, Queue>;
    void addOrder(const Order& order);
    [[nodiscard]] std::optional<std::pair<double, double>> bestPrices() const;
    std::optional<std::pair<Order, Order>> match(); // matches trades while bestBid >= bestAsk
    // decrease quantity if needed or remove exhausted trades.
    // return pair<bid,ask> or std::nullopt if no trades
    void purgeExpired(const std::chrono::steady_clock::time_point& now);
    [[nodiscard]]double spread() const;
    [[nodiscard]]double getDepth(int levels) const;
private:
    // optional std::vector<Trade> that stores transaction history.
    Bids bids;
    Asks asks;
};