#include <iostream>
#include "AggressiveAgent.h"
#include "ConservativeAgent.h"
#include "Market.h"
// #include "Order.h"

constexpr int ITERATIONS_COUNT = 10000;

[[deprecated("use CommonFunctions::countObjectType instead")]]
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

        // market.marketStatsLogger.openFile("", true);

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
        proportionalAgentPrePopulation(100, 0.3);
        //std::print("Num of aggressive agent: {}\n", numOfAggressiveAgents(agents));
        // std::cout <<"Num of aggressive agent: " << numOfAggressiveAgents(agents) << std::endl;

        auto [aggressiveAgentsCount, agentsCount] = CommonFunctions::countObjectType(agents, AgentType::AGGRESSIVE);
        std::cout <<"Num of aggressive agent: " << aggressiveAgentsCount << '\n';

        market.logState();
        // market.run(ITERATIONS_COUNT);
        // for (int i = 0; i < ITERATIONS_COUNT; i++) {
        //     market.step();
        //     std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(10)));
        // }

        market.run(500);
        market.triggerCrash(0.3); // triggering crash
        market.run(500);
        market.logState();

        std::cout << "Number of agents in simulation: " <<  agentsCount << '\n';
        std::cout << "===============\nSimulation done\n===============\n";

    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}
