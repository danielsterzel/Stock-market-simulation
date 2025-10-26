#pragma once
#include <chrono>

struct Order {
  int id;
  bool isBid;
  double price;
  int quantity;
  std::chrono::steady_clock::time_point timestamp;
  std::chrono::milliseconds ttl;

};

