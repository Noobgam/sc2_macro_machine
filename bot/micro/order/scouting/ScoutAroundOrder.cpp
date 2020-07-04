#include "ScoutAroundOrder.h"

using std::vector;

ScoutAroundOrder::ScoutAroundOrder(CCBot &bot, Squad* squad, CCPosition position) : Order(bot, squad), m_target_position(position) { }

void ScoutAroundOrder::onStart() {
    emp = new ExactDistanceMap{m_bot, m_target_position, 100};
    vector <CCPosition> positions;
    for (auto& lr : emp->m_dist) {
        positions.push_back()
    }
}

void ScoutAroundOrder::onStep() {
    for (auto&& lr : emp->m_dist) {
        m_bot.Map().drawTile(lr.first.first, lr.first.second);
    }
}
