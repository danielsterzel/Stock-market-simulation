#include <print>
// #include "Logger.h"
#include "Order.h"
#include "OrderBook.h"

int main() {
    using namespace std::chrono;

    OrderBook orderBook;

    auto now = steady_clock::now();

    Order bid1(OrderType::LIMITORDER, 1, true, 100.0, 10, now, 10s);
    Order bid2(OrderType::LIMITORDER, 2, true, 99.5, 20, now, 10s);
    Order ask1(OrderType::LIMITORDER, 3, false, 100.5, 20, now, 10s);
    Order ask2(OrderType::LIMITORDER, 4, false, 101.0, 10, now, 10s);

    orderBook.addOrder(bid1);
    orderBook.addOrder(bid2);
    orderBook.addOrder(ask1);
    orderBook.addOrder(ask2);

    if (const auto best = orderBook.bestPrices()) {
        auto [bidPrice, askPrice] = *best;
        std::print("Best bid price: {} \nBest ask price {}\n", bidPrice, askPrice);
    } else {
        std::print("Book is empty");
    }

    std::print("Spread: {}\n", orderBook.spread());

    std::print("Depth (2 levels): {}\n", orderBook.getDepth(2));


    Order aggressiveBuy{OrderType::MARKETORDER, 5, true, 101.0, 5, now, 10s};
    orderBook.addOrder(aggressiveBuy);

    if (auto trade = orderBook.match()) {
        std::print("Trade occurred\n");
        std::print("Bid id: {} | Ask id: {}\n", trade->first.id, trade->second.id);
        std::print("Remaining quantity in bid: {} | Remaining quantity in ask: {}\n", trade->first.quantity,
                   trade->second.quantity);
    }else {
        std::print("no match yet");
    }

    auto futureTimeStamp = now + 20s;
    orderBook.purgeExpired(futureTimeStamp);
    std::print("After purging expired orders\n");

    std::print("Spread: {}\n", orderBook.spread());

    return 0;
}
