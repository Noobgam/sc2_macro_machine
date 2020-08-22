#pragma once

#include <micro/order/Order.h>

class CCBot;
class BaseLocation;

class ScoutEnemyBasesOrder: public Order {
private:
    std::optional<const Unit*> m_scout;
    const std::vector<const BaseLocation*> m_baseLocationsToScout;

    std::vector<const BaseLocation*>::const_iterator m_nextBaseIt;
    void scoutAround();
public:
    ScoutEnemyBasesOrder(CCBot & bot, Squad* squad, const std::vector<const BaseLocation*>& baseLocations);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
    void onUnitRemoved(const Unit* unit) override;
};

