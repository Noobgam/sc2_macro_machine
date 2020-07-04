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

    void onStart() {
        emp = new ExactDistanceMap{m_bot, m_target_position, 100};
    }

    void onStep() {
        for (auto&& lr : emp->m_dist) {
            m_bot.Map().drawTile(lr.first.first, lr.first.second);
        }
    }
};


