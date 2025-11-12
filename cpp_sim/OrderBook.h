#pragma once

#include <map>
#include <deque>
#include <utility>
#include <limits>
#include <algorithm>
#include <functional>

#include "Order.h"
#include "Trade.h" // TODO: Return Trade struct instead of std::optional<std::pair<Order, Order>>


// bids
// 100.5 [order1, order2, order3]
//  99.4 [order4 ...]

class OrderBook {
public:
    using OrderPurgeCallback = std::function<void(const Order& order)>;
    using Queue = std::deque<Order>;
    using Bids = std::map<double, Queue, std::greater<>>;
    using Asks = std::map<double, Queue>;
    //using Trade = std::optional<std::pair<Order, Order>>;

    std::vector<Trade> processOrder(Order& incomingOrder, const std::chrono::steady_clock::time_point& now);

    void addOrder(const Order& order);
    [[nodiscard]] std::optional<std::pair<double, double>> bestPrices() const;
    //std::optional<std::pair<Order, Order>> match();
    void purgeExpired(const std::chrono::steady_clock::time_point& now);
    [[nodiscard]] double spread() const;
    [[nodiscard]] double getDepth(int levels) const;
    void setOrderPurgeCallback(OrderPurgeCallback callBack);
private:
    // optional std::vector<Trade> that stores transaction history?
    OrderPurgeCallback logPurgedOrderFunction;
    Bids bids;
    Asks asks;
};