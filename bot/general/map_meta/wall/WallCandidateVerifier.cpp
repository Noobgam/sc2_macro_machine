#include "WallCandidateVerifier.h"

#include <general/model/Common.h>
#include "WallVerifier.h"
#include <general/CCBot.h>
#include <util/Util.h>

struct cmp {
    bool operator()(const CCTilePosition& lhs, const CCTilePosition& rhs) const {
        if (lhs.x != rhs.x) {
            return lhs.x < rhs.x;
        }
        return lhs.y < rhs.y;
    }
};

WallCandidateVerifier::WallCandidateVerifier(
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

bool WallCandidateVerifier::verifyPlacement(
        // {{lx, ly}, type} - position of a left-bottom most tile, and building size
        const std::vector<std::pair<std::pair<int,int>, BuildingType>>& alreadyPlaced,
        [[maybe_unused]] const std::vector<BuildingType>& buildingsLeft
) const {
    if (alreadyPlaced.empty()) {
        return true;
    }
    if (alreadyPlaced.back().second != BuildingType::PoweringPylon) {
        // current heuristic will already either cut at pylon or return true
        return true;
    }
    std::set<CCTilePosition, cmp> blockedByWall;
    int lx = std::numeric_limits<int>::max();
    int ly = std::numeric_limits<int>::max();
    int rx = std::numeric_limits<int>::min();
    int ry = std::numeric_limits<int>::min();
    for (auto& building : alreadyPlaced) {
        int x = building.first.first;
        int y = building.first.second;
        if (building.second == BuildingType::ThreeByThree) {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    blockedByWall.insert(CCTilePosition{x + i, y + j});
                }
            }
            lx = std::min(lx, x - 1);
            ly = std::min(ly, y - 1);
            rx = std::max(rx, x + 3);
            ry = std::max(ry, y + 3);
        } else if (building.second == BuildingType::PoweringPylon) {
            for (int i = -7; i <= 7; i++) {
                for (int j = -7; j <= 7; ++j) {
                    float xi = x - i + .5;
                    float yj = y - j + .5;
                    if (!m_mapMeta.isBuildable(x - i, y - j)) continue;
                    const int SIZE = 9;
                    // 9 =
                    //   6.5 + 1.5 (half a gateway) + 1 (a single unit gap to allow walls with 1 empty tile)
                    if (Util::Dist({x + .0f, y + .0f}, {xi, yj}) <= SIZE) {
                        blockedByWall.insert(CCTilePosition{x - i, y - j});
                    }
                }
            }

        }
    }

    auto&& bases = m_mapMeta.getBaseLocations();
    auto it = std::find_if(bases.begin(), bases.end(), [this](auto& loc) {
        return loc.getBaseId() == m_enemyStartBaseLocationId;
    });
    // move from enemy location to one of base-box tiles
    auto startTile = Util::GetTilePosition((*it).pos);

    auto m_width = m_mapMeta.width();
    auto m_height = m_mapMeta.height();
    auto m_dist = std::vector<std::vector<bool>>(m_width, std::vector<bool>(m_height));
    std::vector<CCTilePosition> m_sortedTiles;
    m_sortedTiles.push_back(startTile);

    m_dist[(int)startTile.x][(int)startTile.y] = true;

    auto&& myBase = *std::find_if(bases.begin(), bases.end(), [this](auto& loc) {
        return loc.getBaseId() == m_baseLocationId;
    });
    auto&& distances = m_mapMeta.getDistanceMap(myBase.pos);
    bool baseReachable = false;
    for (size_t fringeIndex=0; fringeIndex < m_sortedTiles.size(); ++fringeIndex)
    {
        int x = m_sortedTiles[fringeIndex].x;
        int y = m_sortedTiles[fringeIndex].y;
        int dist = distances.getDistance(CCPosition(x + .5f, y + .5f));
        if (dist != -1 && dist <= 6) {
            baseReachable = true;
            break;
        }

        // check every possible child of this tile
        for (size_t a=0; a < LEGAL_ACTIONS; ++a)
        {
            CCTilePosition nextTile(x + actionX[a], y + actionY[a]);
            // if the new tile is inside the map bounds, is walkable, and has not been visited yet, set the distance of its parent + 1
            if (m_mapMeta.isWalkable(nextTile.x, nextTile.y) && m_dist[(int)nextTile.x][(int)nextTile.y] == false)
            {
                if (blockedByWall.count(nextTile)) continue;
                m_dist[(int)nextTile.x][(int)nextTile.y] = true;
                m_sortedTiles.push_back(nextTile);
            }
        }
    }
    return !baseReachable;
}
