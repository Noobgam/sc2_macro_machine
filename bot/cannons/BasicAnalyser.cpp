#include "BasicAnalyser.h"

#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <util/Util.h>

#include <algorithm>
#include <set>

constexpr auto ACCEPTABLE_REALTIME_STUTTER = std::chrono::milliseconds{5};

bool BasicAnalyser::recalculate(const CCBot &bot) {
    if (lastCalculationFuture.valid()) {
        LOG_DEBUG << "UB will happen if you non-atomically modify vectors." << BOT_ENDL;
        auto res = lastCalculationFuture.wait_for(ACCEPTABLE_REALTIME_STUTTER);
        if (res == std::future_status::timeout) {
            LOG_DEBUG << "Dropping request due to timeout" << BOT_ENDL;
            return false;
        } else {
            LOG_DEBUG << "Continue recalculating" << BOT_ENDL;
        }
    }
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
    return true;
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
    PylonPlacement placement = {
            chosenPylons,
            {},
            {}
    };
    if (currentAnalysis->pylonPlacements.count(placement)) {
        // no duplicates should be allowed
        return;
    }
    if (!fastCheck()) {
        return;
    }
    auto&& slowRes = slowCheck();
    if (slowRes.isSuccess()) {
        PylonPlacement finalPlacement = {
                chosenPylons,
                {},
                std::move(slowRes.cannonPlacements)
        };
        currentAnalysis->pylonPlacements.insert(std::move(finalPlacement));
    }
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

namespace {
    using namespace std;
    struct hash_pair {
        template<class T1, class T2>
        size_t operator()(const pair <T1, T2> &p) const {
            auto hash1 = hash<T1> {}(p.first * 10000);
            auto hash2 = hash<T2> {}(p.second);
            return hash1 ^ hash2;
        }
    };
}

bool BasicAnalyser::fastCheck() const {
    std::unordered_set <std::pair<int,int>, hash_pair> cells;
    std::unordered_set <std::pair<int,int>, hash_pair> visited;
    std::vector<std::pair<int,int>> fringe;
    auto add_cell = [&cells, this] (int x, int y) {
        if (walkable[x][y]) {
            cells.insert({x, y});
        }
    };
    for (auto pylon : chosenPylons) {
        int x = pylon.x;
        int y = pylon.y;
        for (int i = x - 1; i <= x + 2; ++i) {
            add_cell(i, y - 1);
            add_cell(i, y + 2);
        }
        for (int i = y - 1; i <= y + 2; ++i) {
            add_cell(x - 1, i);
            add_cell(x + 2, i);
        }
    }
    if (cells.empty()) {
        // if literally everything around pylons is blocked. Probably this
        return false;
    }

    int dx[4] = {-1,0,1,0};
    int dy[4] = {0,1,0,-1};
    auto start = *cells.begin();
    fringe.push_back(start);
    visited.insert(start);
    for (int i = 0; i < fringe.size(); ++i) {
        int x = fringe[i].first;
        int y = fringe[i].second;
        for (int s = 0; s < 4; ++s) {
            int tox = x + dx[s];
            int toy = y + dy[s];
            if (!cells.count({tox, toy})) {
                continue;
            }
            auto&& lr = visited.insert({tox, toy});
            if (lr.second) {
                fringe.push_back({tox, toy});
            }
        }
    }
    if (visited.size() == cells.size()) {
        return false;
    }
    return true;
}

BasicAnalyser::SlowCheckResult BasicAnalyser::slowCheck() {
    visitedSlow.assign(m_width, std::vector<int>(m_height, 0));
    visitedComp = 0;
    vector <int> compIds;
    vector <pair<int,int>> compRoots;
    auto start_dfs = [this, &compRoots, &compIds] (int x, int y) {
        if (walkable[x][y] && visitedSlow[x][y] == 0) {
            ++visitedComp;
            auto lr = dfs(x, y);
            if (!lr.second && lr.first >= 4) {
                compRoots.emplace_back(x, y);
                compIds.push_back(visitedComp);
            }
        }
    };
    for (auto pylon : chosenPylons) {
        int x = pylon.x;
        int y = pylon.y;
        for (int i = x - 1; i <= x + 2; ++i) {
            start_dfs(i, y - 1);
            start_dfs(i, y + 2);
        }
        for (int i = y - 1; i <= y + 2; ++i) {
            start_dfs(x - 1, i);
            start_dfs(x + 2, i);
        }
    }
    if (compRoots.size() == 0) {
        // think about ramp walls.
        return {};
    }
    for (int i = 0; i < compRoots.size(); ++i) {
        int compId = compIds[i];
        const auto [x, y] = compRoots[i];
        auto&& cannons = dfsCannonPlacement(x, y, compId);
        if (!cannons.empty()) {
            return {
                cannons
            };
        }
    }
    // there is a check missing that the cannon can actually be placed with these 3 pylons.
    // might come into play in a ramp-wall (no power upwards/no placement in range)
    return {};
}

std::pair<int, bool> BasicAnalyser::dfs(int x, int y) {
    if (!isRelevantTile[x][y]) {
        return {0, true};
    }
    int total = 1;
    bool nonRelevant = false;
    visitedSlow[x][y] = visitedComp;
    int dx[8] = {-1,-1,-1,0,1,1,1,0};
    int dy[8] = {-1,0,1,1,1,0,-1,-1};
    for (int i = 0; i < 8; ++i) {
        int tox = x + dx[i];
        int toy = y + dy[i];
        if (visitedSlow[tox][toy] == 0 && canWalk(x, y, tox, toy)) {
            auto lr = dfs(tox, toy);
            total += lr.first;
            nonRelevant |= lr.second;
        }
    }
    return {total, nonRelevant};
}

std::vector<CCTilePosition> BasicAnalyser::dfsCannonPlacement(int x, int y, int comp) {
    if (!isRelevantTile[x][y]) {
        return {};
    }
    visitedSlow[x][y] = -visitedSlow[x][y];
    int dx[8] = {-1,-1,-1,0,1,1,1,0};
    int dy[8] = {-1,0,1,1,1,0,-1,-1};
    std::vector<CCTilePosition> placementsHere;
    for (int i = 0; i < 8; ++i) {
        int tox = x + dx[i];
        int toy = y + dy[i];
        if (visitedSlow[tox][toy] == comp && canWalk(x, y, tox, toy)) {
            auto&& vv = dfsCannonPlacement(tox, toy, comp);
            if (!vv.empty()) {
                Util::MoveAppend(vv, placementsHere);
            }
        }
    }
    // for every possible vertex check [-1 -1] placement
    bool canPlace = true;
    for (int i = 0; canPlace && i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (abs(visitedSlow[x - i][y - j]) != comp) {
                canPlace = false;
                break;
            }
        }
    }
    if (canPlace) {
        placementsHere.emplace_back(x - 1, y - 1);
    }
    return placementsHere;
}

bool BasicAnalyser::canWalk(int fromx, int fromy, int tox, int toy) const {
    // TODO: take minerals into account
    return max(abs(fromy - toy), abs(fromx - tox)) <= 1 && walkable[tox][toy];
}
