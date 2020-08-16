#pragma once

#include <micro/order/Order.h>

class GroupAroundOrder : public Order {
  bool attackWhileMoving;
  CCPosition targetPosition;

  float getCircleRadius() const;
 public:
  GroupAroundOrder(CCBot &bot, Squad *squad, CCPosition targetPosition, bool attackWhileMoving);

  void onStep() override;
  void draw() const override;
};


