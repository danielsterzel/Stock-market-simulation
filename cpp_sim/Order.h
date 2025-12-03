#pragma once
#include <chrono>
#include "OrderType.h"

struct Order {
    static inline int nextInt = 0;

    OrderType orderType;
    int id;
    bool isBid;
    double price;
    int quantity;
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::milliseconds ttl;

    Order(const OrderType agentOrderType, const bool isOrderABid,
          const double orderPrice, const int orderQuantity,
          const std::chrono::steady_clock::time_point &orderTimestamp,
          const std::chrono::milliseconds orderTTL): orderType(agentOrderType), id(nextInt++), isBid(isOrderABid),
                                                     price(orderPrice),
                                                     quantity(orderQuantity), timestamp(orderTimestamp), ttl(orderTTL) {
    }

    [[nodiscard]] bool expired(const std::chrono::steady_clock::time_point &now) const noexcept {
        return now - timestamp > ttl;
    }
    static constexpr bool isEmpty = false;
};


struct EmptyOrder : Order {
    EmptyOrder()
    : Order(OrderType::CANCELORDER, false, 0.0, 0,
            std::chrono::steady_clock::time_point{},
            std::chrono::milliseconds{}) {}
    static constexpr bool isEmpty = true;
};

inline std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << "id: " << order.id
    << ", type: " << order.orderType
    << ", side: " << (order.isBid ? "BID" : "ASK")
    << ", price: " << order.price
    << ", quantity: " << order.quantity
    << ", ttl: " << order.ttl.count() << "ms";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const EmptyOrder& eo) {
    os << "[EmptyOrder id=" << eo.id << "]";
    return os;
}
