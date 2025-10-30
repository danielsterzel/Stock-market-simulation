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
    void logState();
    AgentContainer& getAgentContainer();
    static inline Logger logger{};


private:
    OrderBook orderBook;
    AgentContainer agents;
    std::chrono::steady_clock::time_point now;
    double initialPrice = 100;
    std::mt19937 generator{std::random_device{}()};
    // std::vector<MarketStats>;

    double fundamentalValue = 100.0;
    std::normal_distribution<double> driftDist{0.0, 0.05};

};
