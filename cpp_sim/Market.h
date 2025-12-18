#pragma once

#include "OrderBook.h"
#include <vector>
#include <deque>
#include "Agent.h"
#include "Logger.h"

class Market {
public:
    using AgentContainer = std::vector<std::unique_ptr<Agent>>;
    Market();

    void loadRealData(const std::string& csvPath);

    void step();
    void run(size_t steps, const std::string& logFileName);
    void logState() const;
    AgentContainer& getAgentContainer();

    mutable Logger marketStatsLogger = {};
    mutable Logger agentActionLogger = {};

    void logLiveState() const;
    void triggerCrash(double severity);
    const std::vector<Trade>& getTradeHistory() const;
    ~Market();


private:
    double calculateVolatility();
    double calculateSlippage(const std::vector<Trade>& trades, double benchmarkPrice);

    OrderBook orderBook;
    AgentContainer agents;
    std::chrono::steady_clock::time_point now;

    double initialPrice = 100;
    double fundamentalValue = 100.0;

    std::vector<double> realPricePath;
    size_t currentDataIndex = 0;

    std::mt19937 generator{std::random_device{}()};
    std::vector<Trade> tradeHistory;
    // std::vector<MarketStats>;

    std::normal_distribution<double> driftDist{0.0, 0.05};

    MarketStats currentStats;
    std::deque<double> priceHistoryWindow;
};
