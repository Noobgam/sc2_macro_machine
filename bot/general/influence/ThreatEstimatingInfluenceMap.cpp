#include "ThreatEstimatingInfluenceMap.h"
#include <algorithm>
#include <general/ThreatAnalyzer.h>

using std::max, std::min;

void ThreatEstimatingInfluenceMap::addInfluence(
        const sc2::UnitTypeData& typeData,
        CCPosition position,
        CCPlayer player,
        ObservedTech tech
) {
    float maxRange = 0;
    for (auto weapon : typeData.weapons) {
        maxRange = std::max(maxRange, weapon.range);
    }
    int lx = max(0.f, position.x - maxRange), rx = min(position.x + maxRange + 0.99f, map_width + .0f);
    int ly = max(0.f, position.y - maxRange), ry = min(position.y + maxRange + 0.99f, map_height + .0f);
    float threat = ThreatAnalyzer::getUnitTypeThreat()
    for (int x = lx; x <= rx; ++x) {
        for (int y = ly; y <= ry; ++y) {
            if (player == Players::Self) {
                allyInfluence +=
            }
        }
    }
    return
}
