#pragma once

#include <optional>
#include <general/model/Unit.h>
#include <general/ThreatAnalyzer.h>
#include <util/Util.h>

namespace MicroUtil {

    std::optional<const Unit*> findUnitWithHighestThreat(
        const Unit* unit,
        const CCBot& bot
    );

    std::optional<const Unit*> findUnitWithHighestThreat(
        const Unit* unit,
        float range,
        const CCBot& bot
    );
};


