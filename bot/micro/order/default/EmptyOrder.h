#pragma once

#include "../Order.h"

class EmptyOrder : public Order {
public:
    explicit EmptyOrder(CCBot & bot, Squad* squad);

    void onStep() override;
  void onStart() override;
};
