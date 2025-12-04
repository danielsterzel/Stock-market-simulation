//
// Created by Daniel Sterzel on 27/10/2025.
//
#include "Market.h"

#include <iostream>
// #include <print>

Market::Market() {
    now = std::chrono::steady_clock::now();
    orderBook.setOrderPurgeCallback([this](const Order& o) {
        agentActionLogger.writeAction("PURGED", o);
    });
    initialPrice = 100.0;
}

void Market::step() {
    static MarketStats lastStats { 99.5, 100.5, 100.0, 1.0, 0.0 };
    MarketStats marketStats = lastStats;

    static std::default_random_engine rng(std::random_device{}());
    static std::normal_distribution<double> drift(0.0, 0.05);
    fundamentalValue += drift(rng);

    marketStats.midPrice = fundamentalValue;
    marketStats.bestBid  = fundamentalValue - 0.5;
    marketStats.bestAsk  = fundamentalValue + 0.5;
    marketStats.spread = marketStats.bestAsk - marketStats.bestBid;
    marketStats.currentDepthLevel = 0.0;


    if (auto bestPrices = orderBook.bestPrices(); bestPrices) {
        marketStats.bestBid = bestPrices->first;
        marketStats.bestAsk = bestPrices->second;
        marketStats.midPrice = (marketStats.bestBid + marketStats.bestAsk) / 2.0;
        marketStats.spread = orderBook.spread();
        marketStats.currentDepthLevel = orderBook.getDepth(5);
    }

    for (const auto &agentPtr : agents) {
        Order order = agentPtr->generateAction(marketStats, now);
        // Logowanie akcji agenta
        agentActionLogger.writeAction(agentPtr->getType(), order);

        if (order.quantity > 0) {
            std::vector<Trade> newTrades = orderBook.processOrder(order, now);
            if (!newTrades.empty()) {
                tradeHistory.insert(tradeHistory.end(), newTrades.begin(), newTrades.end());
            }
        }
    }

    orderBook.purgeExpired(now);

    lastStats = marketStats;
    now += std::chrono::milliseconds(static_cast<long long>(1));

    //logLiveState();
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

void Market::logLiveState() const {
    if (auto bestPrices = orderBook.bestPrices(); bestPrices) {
        const auto [bestBid, bestAsk] = *bestPrices;
        std::cout << bestBid << "," << bestAsk << "\n";
        std::cout << std::flush;
    }
}

const std::vector<Trade>& Market::getTradeHistory() const {
    return tradeHistory;
}

Market::AgentContainer& Market::getAgentContainer() {
    return agents;
}

void Market::triggerCrash(double severity) {
    if (severity > 1.0) severity = 1.0;
    std::cout << "Creating a crash . . .\n";

    fundamentalValue *= (1.0-severity);

    int removedLevels = 0;
    std::uniform_real_distribution<double> removeChance(0.0, 1.0);

    auto& asks = orderBook.getAsks();
    auto& bids = orderBook.getBids();

    auto purgeAsks = [&] {
        for (auto it = asks.begin(); it != asks.end();) {
            if (removeChance(generator) < severity) {
                it = asks.erase(it);
                removedLevels++;
            }
            else {
                ++it;
            }
        }
    };

    auto purgeBids = [&] {
        for (auto it = bids.begin(); it != bids.end(); ) {
            if (removeChance(generator) < severity) {
                it = bids.erase(it);
                removedLevels++;
            }else {
                ++it;
            }
        }
    };

    purgeAsks();
    purgeBids();

    for (auto& [price, queue] : orderBook.getAsks()) {
        for (auto& order: queue) {
            order.price *= (1.0 + severity * 0.01 );
        }
    }

    for (auto& [price, queue] : orderBook.getBids()) {
        for (auto& order: queue) {
            order.price *= (1.0 + severity * 0.01 );
        }
    }
}
Market::~Market() {
    if (agents.empty()) {
        std::cout << "Empty market no agents!";
        return;
    }
    try {
        Logger::saveScenarioConfig(agents, "../logs/ScenarioConfig.json");
    } catch (...) {
        std::cerr << "[WARN] Failed to save scenario config in destructor\n";
    }
}
