#pragma once

#include <general/managers/base/Base.h>
#include "../Order.h"
#include "../../../general/bases/BaseLocation.h"

class CollectVespeneOrder: public Order {
private:
    const Base * m_base;
    std::map<ResourceID, std::vector<const Unit*>> m_assimilatorToWorker;

    void assignWorkers(const  std::set<const Unit*>& workers);
public:
    CollectVespeneOrder(CCBot & bot, Squad* squad, const Base* base);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
    void onUnitRemoved(const Unit* unit) override;
};
