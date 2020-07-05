#include "ScoutAroundOrder.h"
#include <algorithm>
#include "../../../util/LogInfo.h"
#include "../../../util/Util.h"

using std::vector;

ScoutAroundOrder::ScoutAroundOrder(CCBot &bot, Squad* squad, CCTilePosition position) : Order(bot, squad), m_target_position(position) { }

ScoutAroundOrder::ScoutAroundOrder(CCBot &bot, Squad* squad, CCPosition position)
    : Order(bot, squad)
    , m_target_position(CCTilePosition(position.x, position.y)) { }

constexpr static int TSP_ESTIMATE = 4;

vector <CCTilePosition> ScoutAroundOrder::orderTilesPerfectly(CCTilePosition start, vector<CCTilePosition> tilesToVisit) {
    auto cmp = [](const CCTilePosition& lhs, const CCTilePosition& rhs) {
        if (lhs.x != rhs.x) return lhs.x < rhs.x;
        return lhs.y < rhs.y;
    };
    std::sort(tilesToVisit.begin(), tilesToVisit.end(), cmp);
    int bestHeuristic = 100000;
    vector <CCTilePosition> best;
    while (true) {
        int curDist = m_bot.Map().getDistanceMap(start).getDistance(tilesToVisit[0]);
        for (int i = 0; i + 1 < tilesToVisit.size(); ++i) {
            curDist += m_bot.Map().getDistanceMap(tilesToVisit[i]).getDistance(tilesToVisit[i + 1]);
        }
        if (curDist < bestHeuristic) {
            bestHeuristic = curDist;
            best = tilesToVisit;
        }
        if (!std::next_permutation(tilesToVisit.begin(), tilesToVisit.end(), cmp)) break;
    }
    return best;
}

void ScoutAroundOrder::onStart() {
    // TODO: handle more than one unit?
    auto& firstUnit = *m_squad->units().begin();
    auto& type = (*m_squad->units().begin())->getType();
    emp = new ExactDistanceMap{m_bot, m_target_position, 70};
    vector <CCTilePosition> positions;
    positions.reserve(emp->m_dist.size());
    for (auto& lr : emp->m_dist) {
        if (lr.second > 15 && m_bot.Map().isBuildable(lr.first.first, lr.first.second)) {
            // no reason to scout our main base
            positions.push_back(CCTilePosition(lr.first.first, lr.first.second));
        }
    }
    vector <CCTilePosition> keyPoints;
    vector <CCTilePosition> positionsLeft = positions;
    while (!positionsLeft.empty()) {
        vector<pair<int, CCTilePosition>> vv;
        for (auto& pos : positions) {
            vv.push_back({0, pos});
            for (auto&& pL : positionsLeft) {
                if (m_bot.Map().isVisible(pos, pL, type, 3.5)) {
                    vv.back().first++;
                }
            }
        }
        auto it = max_element(vv.begin(), vv.end(), []
                (const pair<int, CCTilePosition>& a, const pair<int, CCTilePosition>& b) {
            return a.first < b.first;
        });
        auto bestPosition = it->second;
        if (it->first <= 7) {
            break;
        }
        positionsLeft.erase(std::remove_if(positionsLeft.begin(), positionsLeft.end(), [this, &bestPosition, &type](const CCTilePosition& position) {
            return m_bot.Map().isVisible(bestPosition, position, type);
        }), positionsLeft.end());
        keyPoints.push_back(bestPosition);
        LOG_DEBUG << positionsLeft.size() << endl;
    }

    // proximity ordering
    auto curpos = CCTilePosition(firstUnit->getPosition().x + .5, firstUnit->getPosition().y + .5);
    vector <CCTilePosition> keypts;
    while (!keyPoints.empty()) {
        auto& map = m_bot.Map().getDistanceMap(curpos);
        auto it = std::min_element(keyPoints.begin(), keyPoints.end(), [&map](const CCTilePosition& lhs, const CCTilePosition& rhs) {
            return map.getDistance(lhs) < map.getDistance(rhs);
        });
        curpos = *it;
        keypts.push_back(curpos);
        keyPoints.erase(it);
    }
    curpos = firstUnit->getTilePosition();
    while (!keypts.empty()) {
        vector<CCTilePosition> firstCoupleOfTiles;
        for (int j = 0; j < keypts.size() && j < TSP_ESTIMATE; ++j) {
            firstCoupleOfTiles.push_back(keypts[j]);
        }
        firstCoupleOfTiles = orderTilesPerfectly(curpos, firstCoupleOfTiles);
        m_keyPoints.push_back(firstCoupleOfTiles[0]);
        curpos = firstCoupleOfTiles[0];
        keypts.erase(std::find(keypts.begin(), keypts.end(), curpos));
    }
    // traveling salesman instead of proximity ordering
    LOG_DEBUG << "KeyPoint count is " << m_keyPoints.size() << endl;
    for (auto& x : m_keyPoints) {
        firstUnit->queueMove(CCPosition(x.x + .5, x.y + .5));
    }
}

void ScoutAroundOrder::onStep() {
    for (int i = 0; i < m_keyPoints.size(); ++i) {
        auto& lr = m_keyPoints[i];
        m_bot.Map().drawTile(lr.x, lr.y, CCColor(255, 0, 0));
        if (i + 1 < m_keyPoints.size()) {
            auto& lr2 = m_keyPoints[i + 1];
            m_bot.Map().drawLine(CCPosition(lr.x + .5, lr.y + .5), CCPosition(lr2.x + .5, lr2.y + .5));
        }
    }
    auto& enemyUnits = m_bot.UnitInfo().getUnits(Players::Enemy);
    for (auto& unit : m_squad->units()) {
        std::optional<const Unit*> unitToAttack;
        for (auto& enemyUnit : enemyUnits) {
            if (Util::Dist(*unit, *enemyUnit) < 15) {
                if (!unitToAttack.has_value()) {
                    unitToAttack = enemyUnit;
                } else {
                    if (unitToAttack.value()->getType().isBuilding()) {
                        // buildings lowest prio
                        unitToAttack = enemyUnit;
                    }
                }
            }
        }
        if (unitToAttack.has_value()) {
            unit->attackUnit(*unitToAttack.value());
        }
    }
    for (auto& unit : m_squad->units()) {
        for (int i = -10; i <= 10; ++i) {
            for (int j = -10; j <= 10; ++j) {

                const CCTilePosition &tile = unit->getTilePosition();
                CCTilePosition close = {tile.x + i, tile.y + j};
                if (!m_bot.Map().isValidTile(close)) continue;
                if (m_bot.Map().isVisible(tile, close, unit->getType())) {
                    m_bot.Map().drawTile(close.x, close.y, CCColor(255, 255, 0));
                }
            }
        }
    }
}
