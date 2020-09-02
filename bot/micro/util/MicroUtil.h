#pragma once

#include <optional>
#include <general/model/Unit.h>
#include <general/ThreatAnalyzer.h>
#include <util/Util.h>

namespace MicroUtil {

    std::optional<const Unit*> findUnitWithHighestThreat(
        const Unit* unit,
        const std::vector<const Unit*>& enemies
    );

    std::optional<const Unit*> findUnitWithHighestThreat(
        const Unit* unit,
        float range,
        const std::vector<const Unit*>& enemies
    );

    std::vector<const Unit*> detectCloseTargets(CCPosition position, int range, const std::vector<const Unit*>& enemies);
};


