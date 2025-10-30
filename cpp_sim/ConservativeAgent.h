#pragma once

#include "Agent.h"

namespace Conservative {
    constexpr double TTL_SHORT = 10;
    constexpr double TTL_LONG = 30;

    class ConservativeAgent final : public Agent {
    public:
        ConservativeAgent();

        Order generateAction(const MarketStats &marketStats, const std::chrono::steady_clock::time_point &now) override;

    private:
        AgentType type = AgentType::CONSERVATIVE;
    };
}
