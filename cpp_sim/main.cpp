#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "AggressiveAgent.h"
#include "ConservativeAgent.h"
#include "MomentumAgent.h"
#include "Market.h"

int main(int argc, char* argv[]) {
    // Domyślne wartości
    std::string csvPath = "../../data/input_prices.csv";
    int steps = 6000;
    int agentsCount = 500;
    float conservativeRatio = 0.6f;
    float aggressiveRatio = 0.2f;
    float momentumRatio = 0.2f;
    std::string logFileName = "../../output/market.csv";
    double crashSeverity = 0.0;

    if (argc >= 7) {
        try {
            csvPath = "../../data/" + std::string(argv[1]);
            steps = std::stoi(argv[2]);
            agentsCount = std::stoi(argv[3]);
            conservativeRatio = std::stof(argv[4]);
            aggressiveRatio = std::stof(argv[5]);
            momentumRatio = std::stof(argv[6]);

            if (argc >= 8) {
                logFileName = "../../output" + std::string(argv[7]);
            }
            if (argc >= 9) {
                crashSeverity = std::stof(argv[8]);
            }
        } catch (const std::exception& e) {
            std::cerr << "Błąd parsowania argumentów: " << e.what() << "\n";
            return 1;
        }
    }

    int conservativeAgents = static_cast<int>(conservativeRatio * agentsCount);
    int aggressiveAgents = static_cast<int>(aggressiveRatio * agentsCount);
    int momentumAgents = static_cast<int>(momentumRatio * agentsCount);

    try {
        Market market;
        market.loadRealData(csvPath);

        auto &agents = market.getAgentContainer();

        auto setupThreeTypesAgents = [&agents](int agg, int cons, int mom) {
            for(int i=0; i<agg; ++i) agents.push_back(std::make_unique<Aggressive::AggressiveAgent>());
            for(int i=0; i<cons; ++i) agents.push_back(std::make_unique<Conservative::ConservativeAgent>());
            for(int i=0; i<mom; ++i) agents.push_back(std::make_unique<Momentum::MomentumAgent>());
        };

        setupThreeTypesAgents(aggressiveAgents, conservativeAgents, momentumAgents);

        std::cout << "--- CONFIGURATION ---\n";
        std::cout << "Steps: " << steps << "\n";
        std::cout << "Agents: " << agentsCount << " (Cons: " << conservativeAgents
                  << ", Agg: " << aggressiveAgents << ", Mom: " << momentumAgents << ")\n";
        std::cout << "Log File: " << logFileName << "\n";
        std::cout << "---------------------\n";

        market.run(steps, logFileName, crashSeverity);


    } catch (const std::exception &e) {
        std::cerr << "CRITICAL ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}