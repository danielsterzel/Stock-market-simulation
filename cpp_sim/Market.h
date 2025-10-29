#pragma once

#include "OrderBook.h"
#include <vector>

#include "Agent.h"
#include "Logger.h"

class Market{

public:
    void step();
    void run(size_t steps);
    void logState();

private:
    OrderBook orderBook;
    std::vector<std::unique_ptr<Agent>> agents;
    std::chrono::steady_clock::time_point now;
    // std::vector<MarketStats>;
    Logger logger;
};
