#include "AggressiveAgent.h"

namespace Aggressive {
    AggressiveAgent::AggressiveAgent() : Agent(AgentType::AGGRESSIVE) {
    }

    Order AggressiveAgent::generateAction(const MarketStats &marketStats,
                                          const std::chrono::steady_clock::time_point &now) {
        if (const double actionDecision = distribution(generator); actionDecision < 0.3) {
            return EmptyOrder{};
        }
        const bool isBuy = (distribution(generator) < 0.5);
        const OrderType type = (distribution(generator) < 0.9) ? OrderType::MARKETORDER : OrderType::LIMITORDER;

        double price = 0.0;
        if (type == OrderType::MARKETORDER) {
            price = isBuy ? marketStats.bestAsk : marketStats.bestBid;
        } else {
            const double timeOffset = (distribution(generator) - 0.5) * marketStats.spread * 0.5;
            price = marketStats.midPrice + (isBuy ? -timeOffset : timeOffset);
        }
        const int quantity = sizeDistribution(generator);

        const double ttlSeconds = std::uniform_real_distribution<double>(TTL_SHORT, TTL_LONG)(generator);

        const std::chrono::milliseconds ttl(static_cast<int>(ttlSeconds * 1000));
        return Order(type, isBuy, price, quantity, now, ttl);
    }
}
