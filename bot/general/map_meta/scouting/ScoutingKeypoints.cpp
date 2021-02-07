#include "ScoutingKeypoints.h"
#include <memory>
#include <general/ExactDistanceMap.h>
#include <algorithm>
#include <util/LogInfo.h>

using std::vector;
using std::make_unique;

static constexpr int R = 8;

vector <CCTilePosition> ScoutingKeyPoints::orderTilesPerfectly(
        const StaticMapMeta& mapMeta,
        CCTilePosition start,
        vector<CCTilePosition> tilesToVisit
) {
    int n = tilesToVisit.size();
    auto cmp = [](const CCTilePosition& lhs, const CCTilePosition& rhs) {
        if (lhs.x != rhs.x) return lhs.x < rhs.x;
        return lhs.y < rhs.y;
    };
    std::sort(tilesToVisit.begin(), tilesToVisit.end(), cmp);
    int bestHeuristic = std::numeric_limits<int>::max();
    vector <CCTilePosition> best;
    vector <vector <int>> dp(n, vector<int>(1<<n));
    while (true) {
        int curDist = mapMeta.getDistanceMap(start).getDistance(tilesToVisit[0]);
        for (int i = 0; i + 1 < tilesToVisit.size(); ++i) {
            curDist += mapMeta.getDistanceMap(tilesToVisit[i]).getDistance(tilesToVisit[i + 1]);
        }
        if (curDist < bestHeuristic) {
            bestHeuristic = curDist;
            best = tilesToVisit;
        }
        if (!std::next_permutation(tilesToVisit.begin(), tilesToVisit.end(), cmp)) break;
    }
    return best;
}

ScoutingKeyPoints ScoutingKeyPoints::getScoutingKeyPoints(const StaticMapMeta &mapMeta, int baseLocationId) {
    auto&& bases = mapMeta.getBaseLocations();
    CCTilePosition targetLocation = std::find_if(bases.begin(), bases.end(), [baseLocationId](auto& base) {
        return base.getBaseId() == baseLocationId;
    })->depotPos;
    auto emp = make_unique<ExactDistanceMap>(
            mapMeta.width(),
            mapMeta.height(),
            targetLocation,
            75,
            [mapMeta](int x, int y) { return mapMeta.isWalkable(x, y); }
    );
    vector <CCTilePosition> positions;
    positions.reserve(emp->m_dist.size());
    for (auto& lr : emp->m_dist) {
        if (lr.second > 35 && mapMeta.isBuildable(lr.first.first, lr.first.second)) {
            // no reason to scout our main base
            positions.push_back(CCTilePosition(lr.first.first, lr.first.second));
        }
    }
    vector <CCTilePosition> keyPoints;
    vector <CCTilePosition> positionsLeft = positions;
    auto is_visible = [&mapMeta](const CCTilePosition& l, const CCTilePosition r) {
        return mapMeta.isVisible(l, r, R);
    };
    while (!positionsLeft.empty()) {
        vector<pair<int, CCTilePosition>> vv;
        for (auto& pos : positions) {
            vv.push_back({0, pos});
            for (auto&& pL : positionsLeft) {
                if (is_visible(pos, pL)) {
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
        positionsLeft.erase(std::remove_if(positionsLeft.begin(), positionsLeft.end(), [&bestPosition, &is_visible](const CCTilePosition& position) {
            return is_visible(bestPosition, position);
        }), positionsLeft.end());
        keyPoints.push_back(bestPosition);
        LOG_DEBUG << positionsLeft.size() << BOT_ENDL;
    }

    vector<size_t> keypointIds;
    for (size_t i = 0; i < keyPoints.size(); ++i) {
        keypointIds.push_back(i);
    }
    // proximity ordering
    auto curpos = targetLocation;
    vector <size_t> keypts;
    while (!keyPoints.empty()) {
        auto map = mapMeta.getDistanceMap(curpos);
        auto it = std::min_element(keypointIds.begin(), keypointIds.end(), [&keyPoints, &map](size_t l, size_t r) {
            auto lhs = keyPoints[l];
            auto rhs = keyPoints[r];
            return map.getDistance(lhs) < map.getDistance(rhs);
        });
        curpos = keyPoints[*it];
        keypts.push_back(*it);
        keypointIds.erase(it);
    }
    return {
        baseLocationId,
        keyPoints,
        keypts
    };
}
