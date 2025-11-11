//
// Created by Daniel Sterzel on 27/10/2025.
//
#include "Market.h"
// #include <print>

Market::Market() {
    now = std::chrono::steady_clock::now();
    orderBook.setOrderPurgeCallback([this](const Order& o) {
        agentActionLogger.writeAction("PURGED", o);
    });
    initialPrice = 100.0;
}

void Market::step() {
    static MarketStats lastStats {
        99.5, 100.5, 100.0, 1.0, 0.0
    };

    MarketStats marketStats = lastStats;

    static std::default_random_engine rng(std::random_device{}());
    static std::normal_distribution<double> drift(0.0, 0.05);
    fundamentalValue += drift(rng);

    marketStats.midPrice = fundamentalValue;
    marketStats.bestBid  = fundamentalValue - 0.5;
    marketStats.bestAsk  = fundamentalValue + 0.5;

    if (const auto bestPrices = orderBook.bestPrices(); bestPrices) {
        marketStats.bestBid = bestPrices->first;
        marketStats.bestAsk = bestPrices->second;
        marketStats.midPrice = (marketStats.bestBid + marketStats.bestAsk) / 2.0;
        marketStats.spread = orderBook.spread();
        marketStats.depth = orderBook.getDepth(5);
    }

    for (const auto &agentPtr : agents) {
        Order order = agentPtr->generateAction(marketStats, now);
        if (order.quantity > 0) {
            orderBook.addOrder(order);
        }
        agentActionLogger.writeAction(agentPtr->getType(), order);
    }


    orderBook.purgeExpired(now);

    bool anyTrade = false;
    while (auto trade = orderBook.match()) {
        const auto& [bidOrder, askOrder] = *trade;

        double tradePrice = (bidOrder.price + askOrder.price) / 2.0;


        initialPrice = tradePrice;

        //logger.logTrade(tradePrice, bidOrder.quantity);
    }

    if (not anyTrade) {
        if (auto bestPrices = orderBook.bestPrices(); bestPrices) {
            const auto [bid, ask] = *bestPrices;
            initialPrice = (bid + ask) / 2.0;
        } else {
            static std::uniform_real_distribution<double> noiseDist(-0.01, 0.01);
            initialPrice += noiseDist(generator);
        }
    }

    lastStats = marketStats;
    now += std::chrono::milliseconds(1);
}

void Market::run(const size_t steps) {
    marketStatsLogger.openFile("../logs/market.csv", true);
    marketStatsLogger.writeCsvHeaders({"Time","BestBid","BestAsk","Spread", "Depth"});
    agentActionLogger.openFile("../logs/marketActions.txt", true);
    for (size_t i = 0; i < steps; ++i) {
        step();
        logState();
    }
}

void Market::logState() const{
    if (auto bestPrices = orderBook.bestPrices(); bestPrices) {
        const auto [bestBid, bestAsk] = *bestPrices;
        const double spread = orderBook.spread();
        const double depth = orderBook.getDepth(5);
        marketStatsLogger.logToCsvFormat(bestBid, bestAsk, spread, depth);
    }
}

Market::AgentContainer& Market::getAgentContainer() {
    return agents;
}
