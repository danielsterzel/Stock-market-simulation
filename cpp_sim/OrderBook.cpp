#include "OrderBook.h"


void OrderBook::addOrder(const Order &order) {
    if (order.isBid) {
        auto& queue = bids[order.price];
        queue.emplace_back(order);
    }else {
        auto& queue = asks[order.price];
        queue.emplace_back(order);
    }
}

std::optional<std::pair<double, double>> OrderBook::bestPrices() const{

    //check is trade expired?

    if (bids.empty() or asks.empty()) {
        return std::nullopt;
    }
    auto bestBidQueue = bids.begin()->second;
    auto bestAskQueue = asks.begin()->second;

    if (bestBidQueue.empty() or bestAskQueue.empty()) {
        return std::nullopt;
    }

    return std::make_pair(bestBidQueue.front().price, bestAskQueue.front().price);
}