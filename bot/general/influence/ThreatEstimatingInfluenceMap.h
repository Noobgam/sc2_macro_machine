#pragma once

#include "AbstractInfluenceMap.h"

// influence map where all units simply threat cells around it.
struct ThreatEstimatingInfluenceMap : AbstractInfluenceMap {
    void addInfluence(
            const sc2::UnitTypeData& typeData,
            CCPosition position,
            CCPlayer player,
            ObservedTech tech
    ) override;
};


