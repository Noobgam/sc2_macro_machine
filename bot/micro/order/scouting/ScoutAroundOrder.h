#pragma once
#include "../Order.h"
#include "../../../general/ExactDistanceMap.h"

// Scouts everything around a point
class ScoutAroundOrder : public Order {
private:
    CCTilePosition m_target_position;
    ExactDistanceMap* emp;
    std::vector<CCTilePosition> m_keyPoints;

public:
    ScoutAroundOrder(CCBot & bot, Squad* squad, CCTilePosition position);
    ScoutAroundOrder(CCBot & bot, Squad* squad, CCPosition position);

    void onStart();

    void onStep();
};


