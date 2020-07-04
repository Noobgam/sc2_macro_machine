#include "ScoutAroundOrder.h"
#include <algorithm>
#include "../../../util/LogInfo.h"

using std::vector;

ScoutAroundOrder::ScoutAroundOrder(CCBot &bot, Squad* squad, CCTilePosition position) : Order(bot, squad), m_target_position(position) { }


void ScoutAroundOrder::onStart() {
    // TODO: handle more than one unit?
    auto& firstUnit = *m_squad->units().begin();
    auto& type = (*m_squad->units().begin())->getType();
    emp = new ExactDistanceMap{m_bot, m_target_position, 100};
    vector <CCTilePosition> positions;
    positions.reserve(emp->m_dist.size());
    for (auto& lr : emp->m_dist) {
        positions.push_back(CCTilePosition(lr.first.first, lr.first.second));
    }
    vector <CCTilePosition> keyPoints;
    vector <CCTilePosition> positionsLeft = positions;
    while (!positionsLeft.empty()) {
        vector<pair<int, CCTilePosition>> vv;
        for (auto& pos : positions) {
            vv.push_back({0, pos});
            for (auto&& pL : positionsLeft) {
                if (m_bot.Map().isVisible(pos, pL, type)) {
                    vv.back().first++;
                }
            }
        }
        auto it = max_element(vv.begin(), vv.end(), []
                (const pair<int, CCTilePosition>& a, const pair<int, CCTilePosition>& b) {
            return a.first < b.first;
        });
        auto bestPosition = it->second;
        positionsLeft.erase(std::remove_if(positionsLeft.begin(), positionsLeft.end(), [this, &bestPosition, &type](const CCTilePosition& position) {
            return m_bot.Map().isVisible(bestPosition, position, type);
        }), positionsLeft.end());
        keyPoints.push_back(bestPosition);
        LOG_DEBUG << positionsLeft.size() << endl;
    }

    // traveling salesman instead of proximity ordering
    LOG_DEBUG << "KeyPoint count is " << keyPoints.size() << endl;
    sort(keyPoints.begin(), keyPoints.end(), [this](const CCTilePosition& lhs, const CCTilePosition& rhs) {
        return emp->m_dist[{lhs.x, lhs.y}] < emp->m_dist[{rhs.x, rhs.y}];
    });
    m_keyPoints = std::move(keyPoints);
    for (auto& x : m_keyPoints) {
        firstUnit->queueMove(CCPosition(x.x + .5, x.y + .5));
    }
}

void ScoutAroundOrder::onStep() {
    for (int i = 0; i < m_keyPoints.size(); ++i) {
        auto& lr = m_keyPoints[i];
        m_bot.Map().drawTile(lr.x + .5, lr.y + .5, CCColor(255, 0, 0));
        if (i + 1 < m_keyPoints.size()) {
            auto& lr2 = m_keyPoints[i + 1];
            m_bot.Map().drawLine(CCPosition(lr.x + .5, lr.y + .5), CCPosition(lr2.x + .5, lr2.y + .5));
        }
    }
}
