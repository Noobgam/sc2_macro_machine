#pragma once

#include <general/managers/base/Base.h>
#include "../Order.h"

class CollectMineralsOrder: public Order {
private:
    const Base* m_base;
    std::map<ResourceID, std::vector<const Unit*>> m_mineralToWorker;

    void assignWorkers(const  std::set<const Unit*>& workers);
    void fixWorker(const Unit* unit, const Unit* mineral) const;
public:
    CollectMineralsOrder(CCBot & bot, Squad* squad, const Base* base);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
    void onUnitRemoved(const Unit* unit) override;

};
