#pragma once

#include "Agent.h"

namespace Conservative {
    constexpr double TTL_SHORT = 10;
    constexpr double TTL_LONG = 30;

    class ConservativeAgent final:public Agent {
        Order generateAction(const MarketStats& marketStats) override;
    private:
        AgentType type = AgentType::CONSERVATIVE;

    };
}



