//
// Created by Daniel Sterzel on 27/10/2025.
//
#include "Market.h"
#include "DataLoader.h"
#include <numeric>
#include <cmath>
#include <iostream>

Market::Market() {
    now = std::chrono::steady_clock::now();
    orderBook.setOrderPurgeCallback([this](const Order& o) {
    agentActionLogger.writeAction("PURGED", o);
    });
    currentStats = {0,0,0,0,0,0,0,0};
}

void Market::loadRealData(const std::string& csvPath) {
    realPricePath = DataLoader::loadPriceHistory(csvPath);
    if (!realPricePath.empty()) {
        fundamentalValue = realPricePath[0];
        initialPrice = fundamentalValue;
        // Reset indeksu przy nowym ładowaniu
        currentDataIndex = 0;
    }
}

double Market::calculateVolatility() {
    constexpr size_t windowSize = 50;
    priceHistoryWindow.push_back(currentStats.midPrice);
    if (priceHistoryWindow.size() > windowSize) {
        priceHistoryWindow.pop_front();
    }

    if (priceHistoryWindow.size() < 2) return 0.0;

    double sum = std::accumulate(priceHistoryWindow.begin(), priceHistoryWindow.end(), 0.0);
    double mean = sum / priceHistoryWindow.size();

    double sq_sum = std::inner_product(priceHistoryWindow.begin(), priceHistoryWindow.end(), priceHistoryWindow.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / priceHistoryWindow.size() - mean * mean);

    return stdev;
}

double Market::calculateSlippage(const std::vector<Trade>& trades, double benchmarkPrice) {
    if (trades.empty()) return 0.0;
    double totalSlippageVolume = 0.0;
    double totalVolume = 0.0;

    for (const auto& trade : trades) {

        double diff = std::abs(trade.price - benchmarkPrice);
        totalSlippageVolume += diff * trade.quantity;
        totalVolume += trade.quantity;
    }
    return (totalVolume > 0) ? (totalSlippageVolume / totalVolume) : 0.0;
}

void Market::step() {
    if (!realPricePath.empty()) {
        if (currentDataIndex < realPricePath.size()) {
            fundamentalValue = realPricePath[currentDataIndex++];
        } else {
            fundamentalValue = realPricePath.back();
        }
    } else {
        static std::normal_distribution<double> drift(0.0, 0.05);
        fundamentalValue += drift(generator);
    }
    currentStats.fundamentalValue = fundamentalValue;

    if (auto bestPrices = orderBook.bestPrices(); bestPrices) {
        currentStats.bestBid = bestPrices->first;
        currentStats.bestAsk = bestPrices->second;
        currentStats.midPrice = (currentStats.bestBid + currentStats.bestAsk) / 2.0;
        currentStats.spread = orderBook.spread();
        currentStats.currentDepthLevel = orderBook.getDepth(5);
    } else {
        currentStats.midPrice = fundamentalValue;
        currentStats.bestBid = fundamentalValue - 0.5;
        currentStats.bestAsk = fundamentalValue + 0.5;
        currentStats.spread = 1.0;
        currentStats.currentDepthLevel = 0.0;
    }

    currentStats.volatility = calculateVolatility();

    double preStepPrice = currentStats.midPrice;
    std::vector<Trade> stepTrades;

    for (const auto &agentPtr : agents) {
        Order order = agentPtr->generateAction(currentStats, now);
        agentActionLogger.writeAction(agentPtr->getType(), order);

        if (order.quantity > 0) {
            std::vector<Trade> newTrades = orderBook.processOrder(order, now);
            if (!newTrades.empty()) {
                tradeHistory.insert(tradeHistory.end(), newTrades.begin(), newTrades.end());
                stepTrades.insert(stepTrades.end(), newTrades.begin(), newTrades.end());
            }
        }
    }

    orderBook.purgeExpired(now);
    currentStats.slippage = calculateSlippage(stepTrades, preStepPrice);

    now += std::chrono::milliseconds(static_cast<long long>(1));
}

void Market::run(const size_t steps, const std::string& logFileName) {
    marketStatsLogger.openFile(logFileName, true);
    marketStatsLogger.writeCsvHeaders({"Time","BestBid","BestAsk","Spread", "Depth", "Fundamental", "Volatility", "Slippage"});
    agentActionLogger.openFile("../logs/marketActions.txt", true);

    size_t runSteps = steps;

    if (!realPricePath.empty() && realPricePath.size() < steps) {
        std::cout << "[INFO] Adjusting simulation steps to data size: " << realPricePath.size() << "\n";
        runSteps = realPricePath.size();
    }

    for (size_t i = 0; i < runSteps; ++i) {
        step();
        logState();
    }
}

void Market::logState() const {
    marketStatsLogger.logToCsvFormat(
    currentStats.bestBid,
    currentStats.bestAsk,
    currentStats.spread,
    currentStats.currentDepthLevel,
    currentStats.fundamentalValue,
    currentStats.volatility,
    currentStats.slippage
    );
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
