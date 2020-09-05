#pragma once

#include "micro/order/Order.h"

class CCBot;

class ProtectPointOrder: public Order {
    CCPosition m_point;

    float getCircleRadius() const;
    int m_PROTECT_RADIUS = 12;
public:
    ProtectPointOrder(CCBot & bot, Squad* squad, CCPosition point);

    void onStep() override;
    void draw() const override;
};
