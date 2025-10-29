#pragma once

#include "Agent.h"
namespace Aggressive {
    constexpr double TTL_SHORT = 0.5;
    constexpr double TTL_LONG = 3;
    class AggressiveAgent final : public Agent{
        AggressiveAgent();
        Order generateAction(const MarketStats& marketStats) override;
    };
}


