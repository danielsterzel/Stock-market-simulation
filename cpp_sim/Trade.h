#pragma once
#include <chrono>

struct Trade{
  double price;
  int quantity;
  int buyerId;
  int sellerId;
  std::chrono::steady_clock::time_point timestamp;
};