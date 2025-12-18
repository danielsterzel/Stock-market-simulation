#include "ConservativeAgent.h"

namespace Conservative {

    ConservativeAgent::ConservativeAgent() : Agent(AgentType::CONSERVATIVE) {}

    Order ConservativeAgent::generateAction(const MarketStats &marketStats,
                                        const std::chrono::steady_clock::time_point &now) {

        if (const double actionDecision = distribution(generator); actionDecision < 0.3) {
            return EmptyOrder{};
        }

    bool isBuy;
    double priceDiff = marketStats.midPrice - marketStats.fundamentalValue;

    if (priceDiff > 0.2) {
        isBuy = false;
    } else if (priceDiff < -0.2) {
        isBuy = true;
    } else {
        isBuy = (distribution(generator) < 0.5);
    }

    const OrderType orderType = (distribution(generator) < 0.9) ? OrderType::LIMITORDER : OrderType::MARKETORDER;

    double price = 0.0;
    if (orderType == OrderType::MARKETORDER) {
        price = isBuy ? marketStats.bestAsk : marketStats.bestBid;
    } else {

        constexpr double maxOffset = 0.5;
        std::normal_distribution<double> noise(0.0, 0.05);

        double offset = ((distribution(generator) - 0.5) * 2.0 * maxOffset) + noise(generator);

        price = marketStats.fundamentalValue + offset;

        constexpr double tickSize = 0.01;
        price = std::round(price / tickSize) * tickSize;
    }

    const int quantity = sizeDistribution(generator);

    const double ttlSeconds = std::uniform_real_distribution<double>(TTL_SHORT, TTL_LONG)(generator);
    const std::chrono::milliseconds ttl(static_cast<long long>(ttlSeconds * 1000));

    return {orderType, isBuy, price, quantity, now, ttl};
}

}
