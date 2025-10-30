//
// Created by Daniel Sterzel on 27/10/2025.
//
#include <print>
#include "Market.h"

Market::Market() {
    now = std::chrono::steady_clock::now();
}

void Market::step() {
    MarketStats marketStats{};
    marketStats.bestBid = initialPrice - 0.5;
    marketStats.bestAsk = initialPrice + 0.5;
    marketStats.midPrice = initialPrice;
    marketStats.spread = 1.0;
    marketStats.depth = 0.0;

    if (auto bestPrices = orderBook.bestPrices(); bestPrices) {
        marketStats.bestBid = bestPrices->first;
        marketStats.bestAsk = bestPrices->second;
        marketStats.midPrice = (marketStats.bestBid + marketStats.bestAsk) / 2.0;
        marketStats.spread = orderBook.spread();
        marketStats.depth = orderBook.getDepth(5);
    }
    // else {
    //     marketStats.bestBid = 0.0;
    //     marketStats.bestAsk = 0.0;
    //     marketStats.midPrice = 0.0;
    //     marketStats.spread = 0.0;
    //     marketStats.depth = 0.0;
    // }

    for (const auto &agentPtr : agents) {
        Order order = agentPtr->generateAction(marketStats, now);
        if (order.quantity > 0) {
            orderBook.addOrder(order);
        }
    }

    orderBook.purgeExpired(now);

    while (auto trade = orderBook.match()) {
        //logger
    }

    now += std::chrono::milliseconds(1);
}

void Market::run(size_t steps) {
    for (size_t i = 0; i < steps; ++i) {
        step();
        logState();
    }
}

void Market::logState() {
    if (auto bestPrices = orderBook.bestPrices(); bestPrices) {
        const auto [bestBid, bestAsk] = *bestPrices;
        const double spread = orderBook.spread();
        const double depth = orderBook.getDepth(5);
        logger.logToCsvFormat(bestBid, bestAsk, spread, depth);
    }
}
Market::AgentContainer& Market::getAgentContainer(){
    return agents;
}
