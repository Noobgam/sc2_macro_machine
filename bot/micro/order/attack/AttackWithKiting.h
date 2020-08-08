#pragma once

#include "../Order.h"
#include <general/CCBot.h>

class AttackWithKiting : public Order {
private:
    CCPosition m_target_position;
    std::set<sc2::Tag> endangered;
    void handleOneUnit(const Unit* unit);
public:
    AttackWithKiting(CCBot & bot, Squad* squad, CCPosition position);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
};


