#include "AggressiveAgent.h"

namespace Aggressive {
    AggressiveAgent::AggressiveAgent() : Agent(AgentType::AGGRESSIVE){}

    Order AggressiveAgent::generateAction(const MarketStats& marketStats) {
        double actionDecision = distribution(generator);
        if (actionDecision < 0.3) {
            return EmptyOrder{};
        }
        bool isBuy = (distribution(generator) < 0.5)
    }

}