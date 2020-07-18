#include <general/model/Common.h>
#include "WallVerifier.h"
#include <general/map_meta/StaticMapMeta.h>
#include <util/Util.h>
#include <vector>
#include "GraphChecker.h"

using std::vector;

struct cmp {
    bool operator()(const CCTilePosition& lhs, const CCTilePosition& rhs) const {
        if (lhs.x != rhs.x) {
            return lhs.x < rhs.x;
        }
        return lhs.y < rhs.y;
    }
};

WallVerifier::WallVerifier(
        const StaticMapMeta& mapMeta,
        int baseLocationId,
        int startBaseLocationId,
        int enemyStartBaseLocationId
)
    : m_mapMeta(mapMeta)
    , m_baseLocationId(baseLocationId)
    , m_startBaseLocationId(startBaseLocationId)
    , m_enemyStartBaseLocationId(enemyStartBaseLocationId)
{}

const static int LEGAL_ACTIONS = 8;
const static int actionX[LEGAL_ACTIONS] = {-1, -1, -1, 0, 1, 1,  1,  0};
const static int actionY[LEGAL_ACTIONS] = {-1,  0,  1, 1, 1, 0, -1, -1};

std::optional<WallPlacement>
WallVerifier::verifyPlacement(
        const std::vector<std::pair<std::pair<int, int>, BuildingType>>& buildings
) {
    auto&& bases = m_mapMeta.getBaseLocations();
    auto it = std::find_if(bases.begin(), bases.end(), [this](auto& loc) {
        return loc.getBaseId() == m_baseLocationId;
    });
    std::set<CCTilePosition, cmp> blockedByWall;
    int lx = std::numeric_limits<int>::max();
    int ly = std::numeric_limits<int>::max();
    int rx = std::numeric_limits<int>::min();
    int ry = std::numeric_limits<int>::min();
    for (auto& building : buildings) {
        int x = building.first.first;
        int y = building.first.second;
        int sz = 2;
        if (building.second == BuildingType::ThreeByThree)
            sz = 3;
        for (int i = 0; i < sz; ++i) {
            for (int j = 0; j < sz; ++j) {
                blockedByWall.insert(CCTilePosition{x + i, y + j});
            }
        }
        lx = std::min(lx, x - 1);
        ly = std::min(ly, y - 1);
        rx = std::max(rx, x + sz);
        ry = std::max(ry, y + sz);
    }

    // fast heuristic:
    bool borderWalkable = true;
    for (int i = lx; borderWalkable && i <= rx; ++i) {
        if (!m_mapMeta.isWalkable(i, ly) || !m_mapMeta.isWalkable(i, ry)) {
            borderWalkable = false;
        }
    }

    for (int j = ly; borderWalkable && j <= ry; ++j) {
        if (!m_mapMeta.isWalkable(lx, j) || !m_mapMeta.isWalkable(rx, j)) {
            borderWalkable = false;
        }
    }
    if (borderWalkable) {
        // if you can move everywhere around the wall - this is not a wall.
        return {};
    }

    auto startTile = Util::GetTilePosition((*it).pos);
    auto m_width = m_mapMeta.width();
    auto m_height = m_mapMeta.height();
    auto m_dist = std::vector<std::vector<int>>(m_width, std::vector<int>(m_height, -1));
    std::vector<CCTilePosition> m_sortedTiles;
    m_sortedTiles.reserve(m_width * m_height);
    m_sortedTiles.push_back(startTile);

    m_dist[(int)startTile.x][(int)startTile.y] = 0;
    GraphChecker checker;
    for (size_t fringeIndex=0; fringeIndex < m_sortedTiles.size(); ++fringeIndex)
    {
        auto & tile = m_sortedTiles[fringeIndex];
        int curDist = m_dist[(int)tile.x][(int)tile.y];

        // check every possible child of this tile
        for (size_t a=0; a < LEGAL_ACTIONS; ++a)
        {
            int dx = actionX[a];
            int dy = actionY[a];
            CCTilePosition nextTile(tile.x + dx, tile.y + dy);
            // if the new tile is inside the map bounds, is walkable, and has not been visited yet, set the distance of its parent + 1
            if (m_mapMeta.isWalkable(nextTile.x, nextTile.y) && !blockedByWall.count(nextTile)) {
                checker.addEdge(tile.x, tile.y, nextTile.x, nextTile.y);
                if (m_dist[(int)nextTile.x][(int)nextTile.y] != -1) continue;
                m_dist[(int)nextTile.x][(int)nextTile.y] = curDist + 1;
                m_sortedTiles.push_back(nextTile);
            }
        }
    }
    CCTilePosition dest = Util::GetTilePosition((*std::find_if(bases.begin(), bases.end(), [this](auto& loc) {
        return loc.getBaseId() == m_enemyStartBaseLocationId;
    })).pos);
    if (m_dist[dest.x][dest.y] == -1) {
        return WallPlacement::fullWall(m_startBaseLocationId, m_baseLocationId, buildings);
    } else {
        // this is not a full wall, could be a wall with 1-unit gap.
        // 2-unit (blocked by pylon/battery)
        auto res = checker.getResult(startTile.x, startTile.y, dest.x, dest.y);
        if (res.articulationPoints.empty()) {
            // no articulation points - not a relevant wall. Could be a diagonal wall with two spots
            return {};
        }
        if (!res.bridges.empty()) {
            // if there are bridges present - this is not a wall.
            // Bridges in the middle of the map probably do not exist
            return {};
        }
        WallPlacement result;
        result.wallType = WallType::WallWithUnitGaps;
        for (auto x : res.articulationPoints) {
            result.gaps.push_back({x, GapType::OneByOne});
        }
        result.startLocationId = m_startBaseLocationId;
        result.baseLocationId = m_baseLocationId;
        result.buildings = buildings;
        return result;
    }
    return {};
}