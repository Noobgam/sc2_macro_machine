#pragma once

#include "../common/TestScenario.h"
#include "../common/TestAgent.h"
#include <sc2api/sc2_gametypes.h>
#include <general/CCBot.h>

#include <memory>

namespace Scenarios {
    static TestScenario CAN_WIN_AGAINST_EMPTY_BOT = TestScenario{
        "CanWinAgainstEmptyBot",
        "SubmarineLE.SC2Map",
        12345,
        true,
        sc2::Race::Protoss,
        sc2::Race::Protoss,
        [] () { return std::make_unique<CCBot>(); },
        [] () { return std::make_unique<TestAgent>(); }
    };
}
