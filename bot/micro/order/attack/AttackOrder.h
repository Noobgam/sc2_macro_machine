#pragma once

#include "../Order.h"

class AttackOrder : public Order {
private:
    CCPosition m_target_position;
public:
    AttackOrder(CCBot & bot, Squad* squad, CCPosition position);

    void onStep() override;
};
