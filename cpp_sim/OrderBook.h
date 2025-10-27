#pragma once

#include <map>
#include <deque>
#include "Order.h"
// #include "Trade.h" // TODO: Return Trade struct instead of std::optional<std::pair<Order, Order>>

class OrderBook {
public:
    using Queue = std::deque<Order>;
    using Bids = std::map<double, Queue, std::greater<>>;
    using Asks = std::map<double, Queue>;
    using Trade = std::optional<std::pair<Order, Order>>;


    void addOrder(const Order& order);
    [[nodiscard]] std::optional<std::pair<double, double>> bestPrices() const;
    std::optional<std::pair<Order, Order>> match();
    void purgeExpired(const std::chrono::steady_clock::time_point& now);
    [[nodiscard]] double spread() const;
    [[nodiscard]] double getDepth(int levels) const;
private:
    // optional std::vector<Trade> that stores transaction history?
    Bids bids;
    Asks asks;
};