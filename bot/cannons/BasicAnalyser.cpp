#include "BasicAnalyser.h"
#include <general/CCBot.h>

void BasicAnalyser::recalculate(const CCBot &bot) {
    if (analysisInProgress()) {
        BOT_ASSERT(false, "UB will happen if you non-atomically modify vectors. Request ignored");
        return;
    }
    auto&& mapTools = bot.Map();
    int w = mapTools.getStaticMapMeta().width();
    int h = mapTools.getStaticMapMeta().height();
    walkable.assign(w, std::vector<bool>(h));
    buildable.assign(w, std::vector<bool>(h));
    minerals.assign(w, std::vector<bool>(h));
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (mapTools.isWalkable(x, y)) {
                walkable[x][y] = true;
            }
            if (mapTools.isBuildable(x, y)) {
                // (TODO) test for creep?
                buildable[x][y] = true;
            }
        }
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

bool BasicAnalyser::analysisInProgress() {
    return lastCalculationFuture.valid();
}

void BasicAnalyser::analyze(const BaseLocation *baseLocation) {
    DistanceMap dm = baseLocation->getDistanceMap();
    // first simple algorithm. There is a pylon that touches the wall.
    // Other pylons are placed in a ring of 3 tiles radius of previous pylon
    // Keep in mind that enemy structures are still considered, since they are actually "walls".
}
