#pragma once

#include "Agent.h"

namespace Aggressive {
    constexpr double TTL_SHORT = 0.5;
    constexpr double TTL_LONG = 3;

    class AggressiveAgent final : public Agent {
    public:
        AggressiveAgent();

        Order generateAction(const MarketStats &marketStats, const std::chrono::steady_clock::time_point &now) override;
    };
}
