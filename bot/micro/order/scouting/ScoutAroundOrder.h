#pragma once
#include "../Order.h"
#include "../../../general/ExactDistanceMap.h"

// Scouts everything around a point
class ScoutAroundOrder : public Order {
private:
    CCPosition m_target_position;
    ExactDistanceMap* emp;

public:
    ScoutAroundOrder(CCBot & bot, Squad* squad, CCPosition position);

    void onStart();

    void onStep();
};


