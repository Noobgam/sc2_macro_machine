#pragma once

#include "../Order.h"
#include <general/CCBot.h>

class AttackWithKiting : public Order {
private:
    CCPosition m_target_position;
    void handleOneUnit(const Unit* unit) const;
public:
    AttackWithKiting(CCBot & bot, Squad* squad, CCPosition position);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
};


