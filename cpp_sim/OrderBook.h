#pragma once

#include <map>
#include <deque>
#include "Order.h"

class OrderBook {
public:
    using Queue = std::deque<Order>;
    using BookSide = std::map<double, Queue, std::greater<>>;

private:
    BookSide bids;
    BookSide asks;
};