#pragma once

#include <micro/order/Order.h>

class ScoutAroundOrder : public Order {
    std::vector<CCTilePosition> m_keyPoints;
    int m_keyPointId;

public:
    ScoutAroundOrder(CCBot & bot, Squad* squad, std::vector<CCTilePosition> keyPoints);

    void onStart();

    void onStep();

};


