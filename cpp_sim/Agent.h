#pragma once

#include <random>
#include "Order.h"
#include "AgentType.h"
#include "MarketStats.h"

class Agent {
public:
    explicit Agent(const AgentType t) : generator(std::random_device{}()), distribution(0.0, 1.0),
                                        sizeDistribution(0, 100),
                                        type(t) {
    }

    [[nodiscard]] AgentType getType() const {
        return type;
    }

    virtual Order generateAction(const MarketStats &marketStats, const std::chrono::steady_clock::time_point &now) = 0;

    virtual ~Agent() = default;

protected:
    std::mt19937 generator; // generate pseudo random numbers
    std::uniform_real_distribution<> distribution; // maps generator output to uniform real distribution
    // e.g [0, 1]
    std::uniform_int_distribution<> sizeDistribution; // maps generator output to integer range

private:
    AgentType type = AgentType::RANDOM;
};
