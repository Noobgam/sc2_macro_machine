#pragma once

#include "micro/order/Order.h"

class CCBot;

class ProtectPointOrder: public Order {
    CCPosition m_point;
public:
    ProtectPointOrder(CCBot & bot, Squad* squad, CCPosition point);

    void onStep() override;
};
