#pragma once

#include <general/CCBot.h>

class ThreatAnalyzer {
    CCBot& m_bot;
public:
    /**
     * Returns a number which represents how big of a threat attacking unit type is for attacked
     * e.g. Immortal is scary for stalker, so this number is going to be big
     */
    static float getUnitTypeThreat(const UnitType& attacker, const UnitType& attacked);
};

