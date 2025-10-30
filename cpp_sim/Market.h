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
    int initialPrice = 100;
    // std::vector<MarketStats>;
};
