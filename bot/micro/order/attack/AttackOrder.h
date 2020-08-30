#pragma once

#include "../Order.h"

class AttackOrder : public Order {
private:
    CCPosition m_target_position;
    std::vector<const Unit*> m_new_units;
public:
    AttackOrder(CCBot & bot, Squad* squad, CCPosition position);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
};
