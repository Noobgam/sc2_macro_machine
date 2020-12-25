#pragma once

#include <micro/order/Order.h>
#include <general/bases/BaseLocation.h>

class ScoutEnemyBaseOrder : public Order {
private:
    std::optional<const Unit*> m_scout;
    const BaseLocation* scoutedLocation;
    CCPosition currentDirection;

    void chooseNewDirection();

public:

    ScoutEnemyBaseOrder(CCBot& bot, Squad* squad, const BaseLocation* baseLocation);
    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
    void onUnitRemoved(const Unit* unit) override;
};


