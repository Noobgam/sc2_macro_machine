#include <general/map_meta/StaticMapMeta.h>
#include <general/map_meta/MapMeta.h>
#include <general/map_meta/wall/WallPlacement.h>

#include <memory>
#include <string>
#include <thread>
#include <iostream>

using namespace std;

struct GroupId {
    int startLocationId;
    int baseLocationId;
    WallType wallType;
    int numberOfBuildings;

    bool operator<(const GroupId &rhs) const {
        if (startLocationId < rhs.startLocationId)
            return true;
        if (rhs.startLocationId < startLocationId)
            return false;
        if (baseLocationId < rhs.baseLocationId)
            return true;
        if (rhs.baseLocationId < baseLocationId)
            return false;
        if (wallType < rhs.wallType)
            return true;
        if (rhs.wallType < wallType)
            return false;
        return numberOfBuildings < rhs.numberOfBuildings;
    }

    bool operator>(const GroupId &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const GroupId &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const GroupId &rhs) const {
        return !(*this < rhs);
    }

    bool operator==(const GroupId &rhs) const {
        return startLocationId == rhs.startLocationId &&
               baseLocationId == rhs.baseLocationId &&
               wallType == rhs.wallType &&
               numberOfBuildings == rhs.numberOfBuildings;
    }

    bool operator!=(const GroupId &rhs) const {
        return !(rhs == *this);
    }
};

GroupId getGroupId(const WallPlacement& wallPlacement) {
    return {
        wallPlacement.startLocationId,
        wallPlacement.baseLocationId,
        wallPlacement.wallType,
        (int)wallPlacement.buildings.size()
    };
}

const size_t LegalActions = 4;
const int actionX[LegalActions] = {1, -1, 0, 0};
const int actionY[LegalActions] = {0, 0, 1, -1};

struct ExtendedWallPlacement {
    WallPlacement wallPlacement;

    // minimum distance from the outside to the pylon
    float pylonExposure;
};

bool comparePlacements(
        const ExtendedWallPlacement& lhs,
        const ExtendedWallPlacement& rhs
) {
    if (lhs.pylonExposure != rhs.pylonExposure) {
        return lhs.pylonExposure > rhs.pylonExposure;
    }
    return lhs.wallPlacement.gaps.size() > rhs.wallPlacement.gaps.size();
}

ExtendedWallPlacement extendWallPlacement(
        StaticMapMeta* staticMapMeta,
        WallPlacement placement
) {
    if (placement.wallType == WallType::FullWall) {
        // this is a bug fixed in https://github.com/Noobgam/sc2_macro_machine/commit/a036f3a4a6ff41beb76f42f8c10f353f294008c7
        swap(placement.baseLocationId, placement.startLocationId);
    }
    set<pair<int,int>> blocked;
    set<pair<int,int>> poweringPylon;
    for (auto& x : placement.buildings) {
        int sz = 2;
        if (x.second == BuildingType::ThreeByThree) {
            sz = 3;
        }
        for (int i = 0; i < sz; ++i) {
            for (int j = 0; j < sz; ++j) {
                int l = x.first.first + i;
                int r = x.first.second + j;
                blocked.insert({l, r});
                if (x.second == BuildingType::PoweringPylon) {
                    poweringPylon.insert({l, r});
                }
            }
        }
    }
    for (auto& x : placement.gaps) {
        int sz = 1;
        if (x.second == GapType::TwoByTwo) {
            // this is a bug fixed in https://github.com/Noobgam/sc2_macro_machine/commit/8456491806dd7b4446f876179068b6038377a2fb
            continue;
        }
        for (int i = 0; i < sz; ++i) {
            for (int j = 0; j < sz; ++j) {
                blocked.insert({x.first.first + i, x.first.second + j});
            }
        }
    }

    std::vector<std::vector<int>> m_dist(staticMapMeta->width(), std::vector<int>(staticMapMeta->height(), -1));
    std::vector<pair<int,int>> queue;
    int q_front = 0;

    for (auto locId : staticMapMeta->getStartLocationIds()) {
        if (locId != placement.startLocationId) {
            auto pos = staticMapMeta->getBaseLocations()[locId].depotPos;
            queue.push_back(std::pair<int,int>(pos.x, pos.y));
        }
    }
    // if you want to support more than 2 spawns - remove the assert
    if (queue.size() != 1) {
        std::cerr << "Queue size is not 1, start locations are invalid!";
        std::terminate();
    }
    assert(queue.size() == 1);

    auto walkable = [&blocked, staticMapMeta](int x, int y) {
        return staticMapMeta->isWalkable(x, y) && !blocked.count({x, y});
    };

    while (q_front < queue.size()) {
        auto tileStart = queue[q_front++];
        int x = tileStart.first;
        int y = tileStart.second;
        int curDist = m_dist[x][y];
        int tmp = 0;

        // check every possible child of this tile
        for (size_t a=0; a<LegalActions; ++a)
        {
            CCTilePosition nextTile(x + actionX[a], y + actionY[a]);
            bool walk = walkable(nextTile.x, nextTile.y);
            int dist = m_dist[nextTile.x][nextTile.y];

            // if the new tile is inside the map bounds, is walkable, and has not been visited yet, set the distance of its parent + 1
            if (walk && dist == -1)
            {
                m_dist[nextTile.x][nextTile.y] = curDist + 1;
                queue.push_back({nextTile.x, nextTile.y});
                ++tmp;
            }
        }
    }

    int distToPylon = 1e9;
    for (auto x : queue) {
        for (auto y : poweringPylon) {
                int xx = y.first - x.first;
                int yy = y.second - x.second;
                int sqr = xx * xx + yy * yy;
                if (distToPylon > sqr) {
                    distToPylon = sqr;
                }
        }
    }
    return {
        placement,
        sqrt(distToPylon + .0f)
    };
}

WallPlacement selectBestWall(
        StaticMapMeta* staticMapMeta,
        std::vector<WallPlacement> placements,
        int l,
        int r
) {
    ExtendedWallPlacement bestPlacement = extendWallPlacement(staticMapMeta, placements[l]);
    for (int i = l + 1; i < r; ++i) {
        ExtendedWallPlacement cur = extendWallPlacement(staticMapMeta, placements[i]);
        if (comparePlacements(cur, bestPlacement)) {
            bestPlacement = cur;
        }
    }
    return bestPlacement.wallPlacement;
}

// implicit contract: wallplacement is valid
// invalid walls produce undefined behaviour
std::vector<WallPlacement> filterWallPlacements(
        StaticMapMeta* staticMapMeta,
        std::vector<WallPlacement> placements
) {
    std::sort(placements.begin(), placements.end(), [](auto&& lhs, auto&& rhs) {
        return getGroupId(lhs) < getGroupId(rhs);
    });
    int l = 0;
    int n = placements.size();
    std::vector<WallPlacement> bestWalls;
    while (l < n) {
        int r = l + 1;
        GroupId start = getGroupId(placements[l]);
        while (r < n && start == getGroupId(placements[r])) {
            ++r;
        }
        bestWalls.push_back(selectBestWall(
                staticMapMeta,
                placements,
                l,
                r
        ));
        l = r;
    }
    return bestWalls;
}

// requires static meta to be already calculated
int main(int argc, char* argv[]) {
    std::vector<string> maps;
    for (int i = 1; i < argc; ++i) {
        maps.push_back(string(argv[i]));
    }
    // multiplied by 10 to utilize cores correctly.
    setVerifierThreadCount(std::thread::hardware_concurrency());
    for (auto mapName : maps) {
        std::unique_ptr<StaticMapMeta> staticMapMeta = StaticMapMeta::getMeta(mapName);
        std::unique_ptr<MapMeta> meta = MapMeta::getMeta(mapName);

        // leave one wall of each available type for all distinct number of
        meta->setWallPlacements(filterWallPlacements(staticMapMeta.get(), meta->getAllWallPlacements()));
        MapMeta::saveToFile(meta, "processed/" + mapName);
    }
    return 0;
}