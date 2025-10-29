#pragma once

#include <random>

#include "Order.h"
#include "AgentType.h"

class Agent {
  public:
    explicit Agent(const AgentType t) : type(t){}
    [[nodiscard]] AgentType getType() const {
        return type;
    }
    virtual Order generateAction(double midPrice, std::chrono::steady_clock::time_point now) = 0;
    virtual ~Agent() = default;
protected:
    std::mt19937 generator;
    std::uniform_real_distribution<> distribution;
    std::uniform_int_distribution<> sizeDistribution;

private:
    AgentType type = AgentType::RANDOM;

};