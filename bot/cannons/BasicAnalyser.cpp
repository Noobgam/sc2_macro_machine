#include "BasicAnalyser.h"
#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <algorithm>

void BasicAnalyser::recalculate(const CCBot &bot) {
//    if (analysisInProgress()) {
//        //BOT_ASSERT(false, "UB will happen if you non-atomically modify vectors. Request ignored");
//        return;
//    }
    auto&& mapTools = bot.Map();
    m_width = mapTools.width();
    m_height = mapTools.height();
    int w = mapTools.getStaticMapMeta().width();
    int h = mapTools.getStaticMapMeta().height();
    statically_walkable.assign(w, std::vector<bool>(h));
    walkable.assign(w, std::vector<bool>(h));
    buildable.assign(w, std::vector<bool>(h));
    minerals.assign(w, std::vector<bool>(h));
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            statically_walkable[x][y] = mapTools.getStaticMapMeta().isStaticallyWalkable(x, y);
            walkable[x][y] = mapTools.isWalkable(x, y);
            buildable[x][y] = mapTools.isBuildable(x, y);
            if (bot.Observation()->HasCreep({x + .5f, y + .5f})) {
                buildable[x][y] = false;
            }
        }
    }
    // depo can be lowered and this will fuck us up real bad
    // many buildings can be lifted, but this should be fine for now
    // this is hard to implement and a questionable choice anyway
    for (auto unit : bot.UnitInfo().getUnits(Players::Enemy, sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT)) {
        int x = unit->getPosition().x - 1;
        int y = unit->getPosition().y - 1;
        markUnbuildable(x, y, 2);
    }
    for (auto unit : bot.UnitInfo().getUnits(Players::Enemy, sc2::UNIT_TYPEID::PROTOSS_NEXUS)) {
        int x = unit->getPosition().x - 4;
        int y = unit->getPosition().y - 4;
        // do not build around nexus for now.
        markUnbuildable(x, y, 9);
    }
    for (auto unit : bot.UnitInfo().getUnits(Players::Enemy, sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER)) {
        int x = unit->getPosition().x - 4;
        int y = unit->getPosition().y - 4;
        // do not build around cc for now.
        markUnbuildable(x, y, 9);
    }
    for (auto mineral : bot.getManagers().getResourceManager().getMinerals()) {
        int y = mineral->getPosition().y;
        int x = mineral->getPosition().x - 1;
        for (int i = 0; i < 2; ++i) {
            minerals[x + i][y] = true;
        }
    }
}

bool BasicAnalyser::analysisReady() {
    return latestAnalysis != NULL;
}

void BasicAnalyser::analyze(const BaseLocation *baseLocation) {
    LOG_DEBUG << "Started analyzing" << BOT_ENDL;
    static int revision = 0;
    // first simple algorithm. There is a pylon that touches the wall.
    // Other pylons are placed in a ring of 3 tiles radius of previous pylon
    // Keep in mind that enemy structures are still considered, since they are actually "walls".

    int x = baseLocation->getDepotActualPosition().x;
    int y = baseLocation->getDepotActualPosition().y;
    tilePosition = {x,y};
    std::vector<CCTilePosition> tiles = {tilePosition};
    isRelevantTile.assign(m_width, std::vector<bool>(m_height));
    std::vector<std::vector<int>> dist(m_width, std::vector<int>(m_height));
    int dx[8] = {-1,-1,-1,0,1,1,1,0};
    int dy[8] = {-1,0,1,1,1,0,-1,-1};
    for (int i = 0; i < tiles.size(); ++i) {
        // copy is intended here
        auto tile = tiles[i];
        isRelevantTile[tile.x][tile.y] = true;
        int dst = dist[tile.x][tile.y];
        if (dst > 20) {
            continue;
        }
        for (int sz = 0; sz < 8; ++sz) {
            int x = tile.x + dx[sz];
            int y = tile.y + dy[sz];
            if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
                continue;
            }
            if (statically_walkable[x][y]) {
                if (!isRelevantTile[x][y]) {
                    tiles.push_back({x, y});
                    dist[x][y] = dst + 1;
                    isRelevantTile[x][y] = true;
                }
            }
        }
    }

    relevantTiles.clear();
    for (auto&& tile : tiles) {
        bool validPylon = true;
        int x = tile.x;
        int y = tile.y;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (!isRelevantTile[x + i][y + j] || !buildable[x + i][y + j]) {
                    validPylon = false;
                    break;
                }
            }
        }
        if (validPylon) {
            relevantTiles.push_back(tile);
        }
    }

    currentAnalysis = new BaseAnalysis();
    currentAnalysis->revision = ++revision;
    currentPylonTarget = 3;
    recursion(relevantTiles);
    auto prev = latestAnalysis.exchange(currentAnalysis);
    if (prev != NULL) {
        delete prev;
    }
    LOG_DEBUG << "Finished analyzing" << BOT_ENDL;
}

void BasicAnalyser::markUnbuildable(int x, int y, int size) {
    int rx = x + size;
    int ry = y + size;
    for (int i = x; i < rx; ++i) {
        for (int j = y; j < ry; ++j) {
            buildable[i][j] = false;
        }
    }
}

void BasicAnalyser::analyzeAsync(const BaseLocation *baseLocation) {
    if (lastCalculationFuture.valid()) {
        // don't want to discard it for now
        lastCalculationFuture.wait();
    }
    lastCalculationFuture = std::async(std::launch::async, [this, baseLocation] () { analyze(baseLocation); });
}

int BasicAnalyser::getAnalysisRevision() {
    return analysisRevision;
}

void BasicAnalyser::recursion(const std::vector<CCTilePosition>& pylonCandidates) {
    if (cutEarly()) {
        return;
    }
    if (chosenPylons.size() == currentPylonTarget) {
        checkCurrentPlacementAndAppend();
        return;
    }
    // every pylon must be placed close to the wall
    for (auto candidate : pylonCandidates) {
        int x = candidate.x;
        int y = candidate.y;
        if (
            !walkable[x - 1][y]
                || !walkable[x + 2][y]
                || !walkable[x - 1][y + 1]
                || !walkable[x + 2][y + 1]
                || !walkable[x][y - 1]
                || !walkable[x][y + 2]
                || !walkable[x + 1][y - 1]
                || !walkable[x + 1][y + 2]
                || minerals[x - 1][y - 1]
                || minerals[x - 1][y + 2]
                || minerals[x + 2][y - 1]
                || minerals[x + 2][y + 2]
            )
        {
            if (addPylon(candidate)) {

                // some major performance optimisation is required here.
                // nice optimization should be to remove pylons that are way too far from this one.
                // Probably 20 valid ones closest to the last placed pylon are the only ones necessary
                recursion(relevantTiles);

                removePylon(candidate);
            }
        }
    }

}

void BasicAnalyser::checkCurrentPlacementAndAppend() {
    // TODO: add final test here
    currentAnalysis->pylonPlacements.push_back({
                                                      chosenPylons,
                                                      {}
    });
}

bool BasicAnalyser::addPylon(CCTilePosition tile) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (!buildable[tile.x + i][tile.y + j]) {
                return false;
            }
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            buildable[tile.x + i][tile.y + j] = false;
            walkable[tile.x + i][tile.y + j] = false;
        }
    }
    chosenPylons.push_back(tile);
    return true;
}

void BasicAnalyser::removePylon(CCTilePosition tile) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            buildable[tile.x + i][tile.y + j] = true;
            walkable[tile.x + i][tile.y + j] = true;
        }
    }
    chosenPylons.pop_back();
}

bool BasicAnalyser::cutEarly() const {
    if (chosenPylons.empty()) {
        return false;
    }
    // this is O(3) but can be optimized with a stack.
    int mnx = std::numeric_limits<int>::max(), mny = std::numeric_limits<int>::max();
    int mxx = std::numeric_limits<int>::min(), mxy = std::numeric_limits<int>::min();
    for (auto pylon : chosenPylons) {
        mnx = std::min(pylon.x, mnx);
        mny = std::min(pylon.y, mny);
        mxx = std::max(pylon.x + 1, mxx);
        mxy = std::max(pylon.y + 1, mxy);
    }
    int value = (mxx - mnx) * (mxy - mny);
    return value > 24;
}
