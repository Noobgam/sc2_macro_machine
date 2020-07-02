#pragma once

#include "../Order.h"

class AttackOrder : public Order {
private:
    CCPosition m_target_position;
public:
    AttackOrder(CCBot & bot, CCPosition position);

    void onStart(Squad* squad) override;
    void onStep(Squad* squad) override;
};
