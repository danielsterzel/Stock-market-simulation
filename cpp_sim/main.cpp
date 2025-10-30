#include <iostream>
#include <print>
#include "AggressiveAgent.h"
#include "ConservativeAgent.h"
#include "Market.h"
#include "Order.h"
// #include "OrderBook.h"

constexpr int ITERATIONS_COUNT = 100;

int numOfAggressiveAgents(const std::vector<std::unique_ptr<Agent> > &agents) {
    int aggressiveAgentCount = 0;
    for (const auto &agent: agents) {
        if (agent->getType() == AgentType::AGGRESSIVE) {
            aggressiveAgentCount++;
        }
    }
    return aggressiveAgentCount;
}

int main() {
    try {
        Market market;

        Market::logger.openFile("../logs/market.csv", true);
        Market::logger.logToCsvFormat("BestBid","BestAsk","Spread", "Depth");
        auto &agents = market.getAgentContainer();

        // auto prepopulateAgentsRandomly = [&agents](const int numOfAgents) {
        //     std::mt19937 generator(std::random_device{}());
        //     std::uniform_real_distribution<> distribution(0, 1.0);
        //
        //     for (int index = 0; index < numOfAgents; index++) {
        //         if (const double agentType = distribution(generator); agentType <= 0.5) {
        //             agents.push_back(std::make_unique<Conservative::ConservativeAgent>());
        //         } else {
        //             agents.push_back(std::make_unique<Aggressive::AggressiveAgent>());
        //         }
        //     }
        // };
        auto proportionalAgentPrePopulation = [&agents](const int agentCount, const double aggressiveAgentProportions) {
            if (aggressiveAgentProportions < 0 or aggressiveAgentProportions > 1.0) {
                std::cerr << "Choose between 0 and 1.0\n";
                return;
            }

            const int finalAggressiveAgentCount = std::floor(agentCount * aggressiveAgentProportions);
            for (int index = 0; index < finalAggressiveAgentCount; index++) {
                agents.push_back(std::make_unique<Aggressive::AggressiveAgent>());
            }

            const int conservativeAgentCount = agentCount - finalAggressiveAgentCount;

            for (int index = 0; index < conservativeAgentCount; index++) {
                agents.push_back(std::make_unique<Conservative::ConservativeAgent>());
            }
        };

        // prepopulateAgents(100);
        proportionalAgentPrePopulation(1000, 0.03);
        std::print("Num of aggressive agent: {}\n", numOfAggressiveAgents(agents));

        market.logState();
        market.run(ITERATIONS_COUNT);
        market.logState();
        std::print("Number of agents in simulation {}\n", agents.size());

        std::print("Simulation done");
    } catch (const std::exception &e) {
        std::print("Error {}\n", e.what());
    }
}
