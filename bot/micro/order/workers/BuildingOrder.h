#pragma once

#include "../Order.h"
#include "../../../macro/buildings/BuildingTask.h"

class BuildingOrder: public Order {
private:
    BuildingTask* m_task;
public:
    BuildingOrder(CCBot & bot, Squad* squad, BuildingTask* task);

    void onStart() override;
    void onStep() override;
};
