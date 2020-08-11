#pragma once

#include <general/managers/base/Base.h>
#include "../Order.h"
#include "../../../general/bases/BaseLocation.h"

class CollectVespeneOrder: public Order {
private:
    const Base * m_base;
    std::map<ResourceID, std::vector<const Unit*>> m_assimilatorToWorker;
    std::vector<const Unit*> m_unassignedWorkers = {};

    void assignWorkers();
    void addActiveAssimilators(const std::vector<std::pair<const Unit *, const Resource *>> &assimilators);
public:
    CollectVespeneOrder(CCBot & bot, Squad* squad, const Base* base);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
    void onUnitRemoved(const Unit* unit) override;
};
