#pragma once

#include <micro/order/Order.h>

class CCBot;
class BaseLocation;

class ScoutEnemyBasesOrder: public Order {
private:
    std::optional<const Unit*> m_scout;
    const std::set<const BaseLocation*> m_baseLocationsToScout;

    std::set<const BaseLocation*>::iterator m_nextBaseIt;
    void scoutAround();
public:
    ScoutEnemyBasesOrder(CCBot & bot, Squad* squad, const std::set<const BaseLocation*>& baseLocations);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
    void onUnitRemoved(const Unit* unit) override;
};

