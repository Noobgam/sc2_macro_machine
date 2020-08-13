#pragma once

#include <micro/order/Order.h>

class GroupAroundOrder : public Order {
  bool attackWhileMoving;
  CCPosition targetPosition;

  void validateFinish();
 public:
  GroupAroundOrder(CCBot &bot, Squad *squad, CCPosition targetPosition, bool attackWhileMoving);

  void onStep() override;
};


