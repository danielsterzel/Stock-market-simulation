//
// Created by Daniel Sterzel on 26/10/2025.
//

#ifndef ORDER_H
#define ORDER_H
#include <chrono>

struct Order {
  int id;
  bool isBid;
  double price;
  int quantity;
  std::chrono::steady_clock::time_point timestamp;
  std::chrono::milliseconds ttl;

};

#endif //ORDER_H
