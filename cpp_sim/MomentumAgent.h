//
// Created by szyro on 17.12.2025.
//

#ifndef STOCK_MARKET_SIMULATION_MOMENTUMAGENT_H
#define STOCK_MARKET_SIMULATION_MOMENTUMAGENT_H
#include "Agent.h"
#include <deque>

namespace Momentum {
    class MomentumAgent final : public Agent {
    public:
        MomentumAgent();
        Order generateAction(const MarketStats &marketStats, const std::chrono::steady_clock::time_point &now) override;
    private:
        std::deque<double> history;
        int memorySize = 10;
    };
} // Momentum

#endif //STOCK_MARKET_SIMULATION_MOMENTUMAGENT_H