#include "WallPlacement.h"
#include "WallVerifier.h"
#include "WallCandidateVerifier.h"

#include <util/LogInfo.h>
#include <util/Util.h>

#include <algorithm>
#include <vector>
#include <set>
#include <csignal>
#include <general/bases/BaseLocation.h>

#include <future>
#include <thread>

using std::vector;

void setVerifierThreadCount(int x) {
    VERIFIER_THREAD_COUNT = x;
}

WallPlacement::WallPlacement() {}

WallPlacement::WallPlacement(
        int baseLocationId,
        int startLocationId,
        WallType wallType,
        std::vector<std::pair<std::pair<int,int>, BuildingType>> buildings,
        std::vector<std::pair<std::pair<int,int>, GapType>>      gaps
) : baseLocationId(baseLocationId)
  , startLocationId(startLocationId)
  , wallType(wallType)
  , buildings(buildings)
  , gaps(gaps)
{}

WallPlacement WallPlacement::fullWall(
        int startLocationId,
        int baseLocationId,
        std::vector<std::pair<std::pair<int,int>, BuildingType>> buildings
) {
    return WallPlacement{startLocationId, baseLocationId, WallType::FullWall, buildings, {}};
}

struct cmp {
    bool operator()(const CCTilePosition& lhs, const CCTilePosition& rhs) const {
        if (lhs.x != rhs.x) {
            return lhs.x < rhs.x;
        }
        return lhs.y < rhs.y;
    }
};

static std::set<std::pair<int,int>> pylons;

void recursion(
        const StaticMapMeta& mapMeta,
        const WallCandidateVerifier& verifier,
        std::set<CCTilePosition, cmp>& candidates,
        // {{lx, ly}, type} - position of a left-bottom most tile, and building size
        std::vector<std::pair<std::pair<int,int>, BuildingType>>& alreadyPlaced,
        // implied contract: buildings are ordered by enum ordering
        std::vector<BuildingType>& buildings,
        std::vector<std::vector<std::pair<std::pair<int,int>, BuildingType>>>& container
) {
    if (buildings.empty()) {
        container.push_back(alreadyPlaced);
        return;
    }
    if (!verifier.verifyPlacement(alreadyPlaced, buildings)) {
        return;
    }
    BuildingType building = buildings.back();
    buildings.pop_back();
    auto beginCandidate = candidates.begin();
    auto endCandidate = candidates.end();
    for (auto it = beginCandidate; it != endCandidate; ++it) {
        int x = it->x;
        int y = it->y;
        alreadyPlaced.push_back({{x, y}, building});
        // place a building so that its left lowest corner overlaps with this tile.
        switch (building) {
            case BuildingType::PoweringPylon: {
                bool badPlacement = false;
                for (int i = 0; i < 2; ++i) {
                    for (int j = 0; j < 2; ++j) {
                        if (!candidates.count(CCTilePosition{x + i, y + j}))
                            badPlacement = true;
                    }
                }
                if (badPlacement) break;
                std::set<CCTilePosition, cmp> almostPoweredCandidates;
                CCPosition pylonCenter{it->x + 1.0f, it->y + 1.0f};
                for (auto it2 = beginCandidate; it2 != endCandidate; ++it2) {
                    // remove candidates used by this building
                    int dx = it2->x - it->x;
                    int dy = it2->y - it->y;
                    if ((dx == 0 || dx == 1) && (dy == 0 || dy == 1)) {
                        continue;
                    }
                    // only one powering pylon by design.
                    // could be extended by placing all powering pylons first and then filtering candidates
                    auto it2Pos = Util::GetPosition(*it2);
                    if (mapMeta.pylonPowers(pylonCenter, 8, it2Pos)) {
                        almostPoweredCandidates.insert(*it2);
                    }
                }
                for (int i = 0; i < 2; ++i) {
                    for (int j = 0; j < 2; ++j) {
                        almostPoweredCandidates.erase(CCTilePosition{x + i, y + j});
                    }
                }
                size_t tmp = container.size();
                recursion(
                        mapMeta,
                        verifier,
                        almostPoweredCandidates,
                        alreadyPlaced,
                        buildings,
                        container
                );
                size_t tmp2 = container.size();
                if (tmp2 != tmp) {
                    pylons.insert({x, y});
                }
                break;
            } case BuildingType::ThreeByThree: {
                bool badPlacement = false;
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        if (!candidates.count({x + i, y + j}))
                            badPlacement = true;
                    }
                }
                if (badPlacement) break;
                auto itf = std::find_if(
                        alreadyPlaced.begin(),
                        alreadyPlaced.end(),
                        [](std::pair<std::pair<int,int>, BuildingType> type) {
                            return type.second == BuildingType::PoweringPylon;
                        }
                );
                CCPosition pylonCenter{itf->first.first + 1.0f, itf->first.second + 1.0f};
                CCPosition gateCenter{it->x + 1.5f, it->y + 1.5f};

                // three by three has to be powered by pylon always.
                if (!mapMeta.pylonPowers(pylonCenter, 6.5, gateCenter)) break;

                // three by three always come up last in list of buildings
                //  so we fix ordering of candidates because why not
                std::set<CCTilePosition, cmp> candidatesLeft;
                auto it2 = it;
                ++it2;
                for (; it2 != endCandidate; ++it2) {
                    candidatesLeft.insert(*it2);
                }
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        candidatesLeft.erase(CCTilePosition{x + i, y + j});
                    }
                }
                recursion(
                        mapMeta,
                        verifier,
                        candidatesLeft,
                        alreadyPlaced,
                        buildings,
                        container
                );
                break;
            } case BuildingType::TwoByTwo: {
                // two by two technically could be built without pylon powering
                // but for the sake of sanity and effectiveness
                // it will require power, so that shield battery could be built here
                break;
            } default:
                assert(false);
        }
        alreadyPlaced.pop_back();
    }
    buildings.push_back(building);
}

std::vector<CCTilePosition> WallPlacement::getTileCandidates(
        const StaticMapMeta &mapMeta,
        int baseLocationId,
        int enemyLocationId
) {
    auto&& bases = mapMeta.getBaseLocations();
    auto it = std::find_if(bases.begin(), bases.end(), [baseLocationId](auto& loc) {
        return loc.getBaseId() == baseLocationId;
    });
    auto myBase = (*it);
    it = std::find_if(bases.begin(), bases.end(), [enemyLocationId](auto& loc) {
        return loc.getBaseId() == enemyLocationId;
    });
    auto enemyBase = (*it);
    auto myBasePos = myBase.pos;
    auto mp_self = mapMeta.getDistanceMap(myBasePos);
    auto mp_enemy = mapMeta.getDistanceMap(enemyBase.pos);
    int dist = mp_enemy.getDistance(myBasePos);
    vector<CCTilePosition> tiles = mp_self.getSortedTiles();
    // only first 600 tiles around the base loc are candidates for building the wall
    constexpr size_t SZ = 600;
    tiles.erase(std::remove_if(tiles.begin(), tiles.end(), [
            &myBase,
            &mapMeta,
            &myBasePos,
            &mp_enemy,
            dist
            ](const CCTilePosition& pos) {
        if (!mapMeta.isBuildable(pos.x, pos.y)) {
            return true;
        }
        float dx = abs(pos.x - myBasePos.x);
        float dy = abs(pos.y - myBasePos.y);
        if (dx <= 4 && dy <= 4) {
            return true;
        }
        if (mp_enemy.getDistance(pos) > dist) {
            return true;
        }

        return false;
    }), tiles.end());
    tiles.resize(std::min(tiles.size(), SZ));
    return tiles;
}

std::vector<WallPlacement> WallPlacement::getWallsForBaseLocation(
        const StaticMapMeta &mapMeta,
        int baseLocationId,
        int startBaseLocationId,
        int enemyStartBaseLocationId
) {
    auto threeBuildings =
            getWallsForBaseLocation(
                    mapMeta,
                    baseLocationId,
                    startBaseLocationId,
                    enemyStartBaseLocationId,
                    {
                            BuildingType::ThreeByThree,
                            BuildingType::ThreeByThree,
                            BuildingType::ThreeByThree,
                            BuildingType::PoweringPylon
                    }
    );

    auto twoBuildings =
            getWallsForBaseLocation(
                    mapMeta,
                    baseLocationId,
                    startBaseLocationId,
                    enemyStartBaseLocationId,
                    {
                            BuildingType::ThreeByThree,
                            BuildingType::ThreeByThree,
                            BuildingType::ThreeByThree,
                            BuildingType::PoweringPylon
                    }
            );
    std::vector<WallPlacement> walls;
    for (auto&& x : threeBuildings) {
        walls.push_back(std::move(x));
    }
    for (auto&& y : twoBuildings) {
        walls.push_back((std::move(y)));
    }
    return walls;
}

std::vector<WallPlacement> WallPlacement::getWallsForBaseLocation(
        const StaticMapMeta &mapMeta,
        int baseLocationId,
        int startBaseLocationId,
        int enemyStartBaseLocationId,
        std::vector<BuildingType> buildings
) {
    int threads = VERIFIER_THREAD_COUNT;
    auto&& bases = mapMeta.getBaseLocations();
    auto it = std::find_if(bases.begin(), bases.end(), [baseLocationId](auto& loc) {
        return loc.getBaseId() == baseLocationId;
    });

    auto basePos = (*it).pos;
    auto mp = mapMeta.getDistanceMap(basePos);

    // tiles is a full list of tiles that could potentially be covered by wall.
    // Make sure to check for buildability via api later.
    vector<CCTilePosition> tiles = getTileCandidates(mapMeta, baseLocationId, enemyStartBaseLocationId);
    std::set<CCTilePosition, cmp> tileCandidates;
    for (auto x : tiles) {
        tileCandidates.insert(x);
    }

    std::vector<std::pair<std::pair<int,int>, BuildingType>> alreadyPlaced;
    std::vector<std::vector<std::pair<std::pair<int,int>, BuildingType>>> container;

    WallCandidateVerifier candidateVerifier{
            mapMeta,
            baseLocationId,
            startBaseLocationId,
            enemyStartBaseLocationId
    };
    recursion(
            mapMeta,
            candidateVerifier,
            tileCandidates,
            alreadyPlaced,
            buildings,
            container
    );
    std::atomic<int> stuffLeft = container.size();
    int initial = container.size();
    std::mutex log_mutex;
    auto validateContainerPart = [&](int l, int r) {
        int cnt = 0;
        WallVerifier verifier{
                mapMeta,
                baseLocationId,
                startBaseLocationId,
                enemyStartBaseLocationId
        };
        std::vector<WallPlacement> placements;
        for (int i = l; i < r; ++i) {
            if (++cnt == 250) {
                int localLeft = (stuffLeft -= cnt);
                int done = (initial - localLeft);
                std::lock_guard<std::mutex> lock(log_mutex);
                LOG_DEBUG << "Done " << done << " of " << initial << endl;
                cnt = 0;
            }
            auto&& placementO = verifier.verifyPlacement(container[i]);
            if (placementO.has_value()) {
                placements.push_back(placementO.value());
            }
        }
        stuffLeft -= cnt;
        cnt = 0;
        return placements;
    };
    std::vector<WallPlacement> placements;
    LOG_DEBUG << "Using " << threads << " threads to calculate meta" << endl;
    if (threads == 1) {
        placements = validateContainerPart(0, container.size());
    } else {
        std::vector<std::future<std::vector<WallPlacement>>> futures;
        for (int i = 0; i < threads; ++i) {
            auto l = (i * container.size()) / threads;
            auto r = ((i + 1) * container.size()) / threads;
            futures.push_back(std::async(std::launch::async, (
                    [&]() {
                        return validateContainerPart(l, r);
                    })
                    )
            );
        }
        for (auto&& x : futures) {
            auto&& vec = x.get();
            placements.insert(placements.end(), vec.begin(), vec.end());
        }
    }
    std::set <std::pair<int,int>> ss;
    for (auto& x : placements) {
        ss.insert(x.buildings[0].first);
    }
    for (auto& x : placements) {
        for (auto y : pylons) {
            if (ss.count(y) == 0) {
                x.gaps.push_back({y, GapType::TwoByTwo});
            }
        }
    }
    LOG_DEBUG << "Found " << ss.size() << " good pylons" << endl;
    LOG_DEBUG << "Found " << pylons.size() - ss.size() << " bad pylons" << endl;

    LOG_DEBUG << "Found " << placements.size() << " valid wall layouts" << endl;
    return placements;
}