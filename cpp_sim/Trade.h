#pragma once
#include <chrono>
#include "Order.h"

struct Trade {
  double price;
  int quantity;
  int bidOrderId;
  int askOrderId;
  std::chrono::steady_clock::time_point timestamp;

  Trade(const Order& bidOrder, const Order& askOrder, int tradeQuantity, const std::chrono::steady_clock::time_point& tradeTimestamp)
      : price(askOrder.price),
        quantity(tradeQuantity),
        bidOrderId(bidOrder.id),
        askOrderId(askOrder.id),
        timestamp(tradeTimestamp) {}
};
