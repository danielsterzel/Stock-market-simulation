#include "OrderBook.h"


void OrderBook::addOrder(const Order &order) {
    if (order.isBid) {
        auto &queue = bids[order.price];
        queue.emplace_back(order);
    } else {
        auto &queue = asks[order.price];
        queue.emplace_back(order);
    }
}

// Function 'std::optional<std::pair<double, double>> OrderBook::bestPrices() const' is not implemented
[[nodiscard]] std::optional<std::pair<double, double> > OrderBook::bestPrices() const {
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

void OrderBook::purgeExpired(const std::chrono::steady_clock::time_point &now) {
    //side means side of the order book either bids or asks.

    auto purgeSide = [&now](auto &side) {
        for (auto it = side.begin(); it != side.end();) {
            auto &queue = it->second;
            queue.erase(
                std::remove_if(queue.begin(), queue.end(),
                               [&](const Order &o) {
                                   return o.expired(now);
                               }),

                queue.end()
            );

            if (queue.empty()) {
                it = side.erase(it);
            } else {
                ++it;
            }
        }
    };
    purgeSide(bids);
    purgeSide(asks);
}

double OrderBook::spread() const {
    const auto bestBidAndAsk = bestPrices();

    if (not bestBidAndAsk) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    const auto[bestBid, bestAsk] = *bestBidAndAsk;
    return bestAsk - bestBid;
}

double OrderBook::getDepth(int levels) const {
    double totalVolume = 0.0;

    auto calculateTotalVolumeForSide = [levels, &totalVolume](auto &side) {
        int count = 0;
        for (auto it = side.begin(); it != side.end() && count < levels; ++it, ++count) {
            const auto &queue = it->second;
            for (const auto &order: queue) {
                totalVolume += order.quantity;
            }
        }
    };

    // each queue is 1 level

    calculateTotalVolumeForSide(bids);
    calculateTotalVolumeForSide(asks);

    return totalVolume;
}
