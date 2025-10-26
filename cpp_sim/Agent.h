#pragma once

#include "Order.h"

class Agent {
  public:
    virtual Order generateAction() = 0;
    virtual ~Agent() = default;
};