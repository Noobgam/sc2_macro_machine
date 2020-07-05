#pragma once

#include "../Order.h"
#include "../../../general/bases/BaseLocation.h"

class CollectMineralsOrder: public Order {
private:
    const BaseLocation* m_baseLocation;
    std::map<CCUnitID, std::vector<const Unit*>> m_mineralToWorker;

    void assignWorkers(const  std::set<const Unit*>& workers);
public:
    CollectMineralsOrder(CCBot & bot, Squad* squad, const BaseLocation* baseLocation);

    void onStart() override;
    void onStep() override;
    void onUnitAdded(const Unit* unit) override;
    void onUnitRemoved(const Unit* unit) override;

};
