#pragma once
#include <chrono>

struct Order {
    int id;
    bool isBid;
    double price;
    int quantity;
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::milliseconds ttl;

    Order(const int id_, const bool isOrderABid,
          const double orderPrice, const int orderQuantity,
          const std::chrono::steady_clock::time_point &orderTimestamp,
          const std::chrono::milliseconds orderTTL): id(id_), isBid(isOrderABid), price(orderPrice),
                                                     quantity(orderQuantity), timestamp(orderTimestamp), ttl(orderTTL) {}

    [[nodiscard]] bool expired(const std::chrono::steady_clock::time_point now) const noexcept {
        return now - timestamp > ttl;
    }
};
