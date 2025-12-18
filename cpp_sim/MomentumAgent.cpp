//
// Created by szyro on 17.12.2025.
//

#include "MomentumAgent.h"
#include <numeric>

namespace Momentum {
    MomentumAgent::MomentumAgent() : Agent(AgentType::MOMENTUM) {}

    Order MomentumAgent::generateAction(const MarketStats &marketStats, const std::chrono::steady_clock::time_point &now) {

        history.push_back(marketStats.midPrice);
        if (history.size() > memorySize) {
            history.pop_front();
        }

        if (history.size() < memorySize) return EmptyOrder{};

        if (distribution(generator) < 0.2) return EmptyOrder{};

        double sum = std::accumulate(history.begin(), history.end(), 0.0);
        double movingAverage = sum / history.size();

        bool isBuy;

        if (marketStats.midPrice > movingAverage + 0.05) {
            isBuy = true;
        }
        else if (marketStats.midPrice < movingAverage - 0.05) {
            isBuy = false;
        } else {
            return EmptyOrder{};
        }

        const int quantity = sizeDistribution(generator);

        double price = isBuy ? marketStats.bestAsk : marketStats.bestBid;

        return {OrderType::MARKETORDER, isBuy, price, quantity, now, std::chrono::milliseconds(static_cast<long long>(100))};
    }
} // Momentum