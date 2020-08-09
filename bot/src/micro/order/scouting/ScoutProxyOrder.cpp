#include "ScoutProxyOrder.h"

ScoutAroundOrder::ScoutAroundOrder(CCBot &bot, Squad *squad, std::vector<CCTilePosition> keyPoints)
    : Order(bot, squad)
    , m_keyPoints(keyPoints)
    , m_keyPointId(0)
{

}

void ScoutAroundOrder::onStart() {
    Order::onStart();
}

void ScoutAroundOrder::onStep() {
    bool unitsBusy = false;
    for (auto unit : m_squad->units()) {
        if (unit->getUnitPtr()->orders.empty()) {
            if (m_keyPointId != m_keyPoints.size()) {
                unit->move(m_keyPoints[m_keyPointId++]);
                unitsBusy = true;
            }
        } else {
            unitsBusy = true;
        }
    }
    if (!unitsBusy && m_keyPointId == m_keyPoints.size()) {
        onEnd();
    }
}
