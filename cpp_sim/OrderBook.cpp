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
    auto purgeSide = [&now, this](auto &side) {
        for (auto it = side.begin(); it != side.end();) {
            auto &queue = it->second;
            const auto firstExpiredOrder = std::remove_if(queue.begin(), queue.end(),
                           [&](const Order &o) {
                               return o.expired(now);
                           });
            if (logPurgedOrderFunction) {
                for (auto logIterator = firstExpiredOrder; logIterator != queue.end(); ++logIterator) {
                    logPurgedOrderFunction(*logIterator);
                }
            }
            queue.erase(firstExpiredOrder, queue.end());

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
    const auto [bestBid, bestAsk] = *bestBidAndAsk;
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

OrderBook::Trade OrderBook::match() {
    if (bids.empty() or asks.empty()) {
        return std::nullopt;
    }
    const auto bestBidIterator = bids.begin();
    const auto bestAskIterator = asks.begin();

    const auto bestBidPrice = bestBidIterator->first;
    const auto bestAskPrice = bestAskIterator->first;

    if (not(bestBidPrice >= bestAskPrice)) {
        return std::nullopt;
    }

    Order &bid = bestBidIterator->second.front(); // we take the first order from order queue
    Order &ask = bestAskIterator->second.front();

    const int tradeQuantity = std::min(bid.quantity, ask.quantity);

    // double tradePrice = ask.price; // this can be useful later for volatility later down the line

    bid.quantity -= tradeQuantity;
    ask.quantity -= tradeQuantity;


    auto removeIfQuantityEqualsNone = [](auto bestMarketQuoteIterator, auto &orderBookSide, auto &order) {
        if (order.quantity == 0 and not bestMarketQuoteIterator->second.empty()) {
            bestMarketQuoteIterator->second.pop_front(); // remove from queue because quantity is 0
        }
        if (bestMarketQuoteIterator->second.empty()) {
            orderBookSide.erase(bestMarketQuoteIterator); // remove queue if we have already processed all orders
        }
    };

    removeIfQuantityEqualsNone(bestBidIterator, bids, bid);
    removeIfQuantityEqualsNone(bestAskIterator, asks, ask);

    return std::make_pair(bid, ask);
}

void OrderBook::setOrderPurgeCallback(OrderPurgeCallback callBack) {
    logPurgedOrderFunction = std::move(callBack);
}
