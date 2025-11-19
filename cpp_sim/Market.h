#pragma once

#include "OrderBook.h"
#include <vector>

#include "Agent.h"
#include "Logger.h"

class Market {
public:
    using AgentContainer = std::vector<std::unique_ptr<Agent>>;
    Market();
    void step();
    void run(size_t steps);
    void logState() const;
    AgentContainer& getAgentContainer();
    mutable Logger marketStatsLogger = {};
    mutable Logger agentActionLogger = {};
    void logLiveState() const;
    void triggerCrash(double severity);
    const std::vector<Trade>& getTradeHistory() const;


private:
    OrderBook orderBook;
    AgentContainer agents;
    std::chrono::steady_clock::time_point now;
    double initialPrice = 100;
    std::mt19937 generator{std::random_device{}()};
    std::vector<Trade> tradeHistory;
    // std::vector<MarketStats>;

    double fundamentalValue = 100.0;
    std::normal_distribution<double> driftDist{0.0, 0.05};

};
