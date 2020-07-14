#include "WallPlacement.h"
#include "WallVerifier.h"
#include "general/CCBot.h"
#include "util/Util.h"
#include <util/LogInfo.h>

#include <algorithm>
#include <vector>
#include <set>
#include <csignal>

using std::vector;

WallPlacement::WallPlacement() {}

WallPlacement::WallPlacement(
        int startBaseLocationId,
        WallType wallType,
        std::vector<std::pair<std::pair<int,int>, BuildingType>> buildings,
        std::vector<std::pair<std::pair<int,int>, GapType>>      gaps
) : startBaseLocationId(startBaseLocationId)
  , wallType(wallType)
  , buildings(buildings)
  , gaps(gaps)
{}

WallPlacement WallPlacement::fullWall(
        int startLocationId,
        std::vector<std::pair<std::pair<int,int>, BuildingType>> buildings
) {
    return WallPlacement{startLocationId, WallType::FullWall, buildings, {}};
}

struct cmp {
    bool operator()(const CCTilePosition& lhs, const CCTilePosition& rhs) const {
        if (lhs.x != rhs.x) {
            return lhs.x < rhs.x;
        }
        return lhs.y < rhs.y;
    }
};

void recursion(
        const CCBot& bot,
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
                    int dx = abs(it2->x - it->x);
                    int dy = abs(it2->y - it->y);
                    if (dx <= 1 && dy <= 1) {
                        continue;
                    }
                    // only one powering pylon by design.
                    // could be extended by placing all powering pylons first and then filtering candidates
                    auto it2Pos = Util::GetPosition(*it2);
                    if (bot.Map().pylonPowers(pylonCenter, 8, it2Pos)) {
                        almostPoweredCandidates.insert(*it2);
                    }
                }
                for (int i = 0; i < 2; ++i) {
                    for (int j = 0; j < 2; ++j) {
                        almostPoweredCandidates.erase(CCTilePosition{x + i, y + j});
                    }
                }
                recursion(
                        bot,
                        almostPoweredCandidates,
                        alreadyPlaced,
                        buildings,
                        container
                );
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
                if (!bot.Map().pylonPowers(pylonCenter, 6.5, gateCenter)) break;

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
                        bot,
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

std::vector<WallPlacement> WallPlacement::getWallsForBaseLocation(
        const CCBot &bot,
        int baseLocationId,
        int startBaseLocationId,
        int enemyStartBaseLocationId
) {
    auto&& bases = bot.Bases().getBaseLocations();
    auto it = std::find_if(bases.begin(), bases.end(), [baseLocationId](const BaseLocation* const loc) {
        return loc->getBaseId() == baseLocationId;
    });

    auto basePos = (*it)->getDepotActualPosition();
    auto mp = bot.Map().getDistanceMap(basePos);

    // tiles is a full list of tiles that could potentially be covered by wall.
    // Make sure to check for buildability via api later.
    vector<CCTilePosition> tiles = mp.getSortedTiles();
    // only first 350 tiles around the base loc are candidates for building the wall
    constexpr size_t SZ = 350;
    tiles.resize(std::min(tiles.size(), SZ));
    tiles.erase(std::remove_if(tiles.begin(), tiles.end(), [&basePos, &bot, &mp](const CCTilePosition& pos) {
        if (!bot.Map().isBuildable(pos)) {
            return true;
        }
        float dx = abs(pos.x - basePos.x);
        float dy = abs(pos.y - basePos.y);
        if (dx <= 2 && dy <= 2) {
            return true;
        }
        return false;
    }), tiles.end());
    std::set<CCTilePosition, cmp> tileCandidates;
    for (auto x : tiles) {
        tileCandidates.insert(x);
    }

    std::vector<std::pair<std::pair<int,int>, BuildingType>> alreadyPlaced;
    std::vector<BuildingType> buildings = {BuildingType::ThreeByThree, BuildingType::ThreeByThree, BuildingType::PoweringPylon};
    std::vector<std::vector<std::pair<std::pair<int,int>, BuildingType>>> container;

    recursion(
            bot,
            tileCandidates,
            alreadyPlaced,
            buildings,
            container
    );
    WallVerifier verifier{
        bot,
        baseLocationId,
        startBaseLocationId,
        enemyStartBaseLocationId
    };
    std::vector<WallPlacement> placements;
    int debug_cnt = 0;
    for (const auto& x : container) {
        ++debug_cnt;
        if (debug_cnt % 100 == 0) {
            LOG_DEBUG << "Processed " << debug_cnt << " of " << container.size() << endl;
        }
        auto&& placementO = verifier.verifyPlacement(x);
        if (placementO.has_value()) {
            placements.push_back(placementO.value());
        }
    }
    LOG_DEBUG << "Found " << placements.size() << " valid wall layouts" << endl;
    return placements;
}