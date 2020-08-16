#include "MicroUtil.h"

namespace MicroUtil {
    std::optional<const Unit*> findUnitWithHighestThreat(
        const Unit* unit,
        const CCBot& bot
    ) {
        // TODO: multiple weapons? Need to take both air and ground ranges into account
        return findUnitWithHighestThreat(
            unit,
            unit->getType().getAttackRange(),
            bot
        );
    }

    std::optional<const Unit*> findUnitWithHighestThreat(
        const Unit* unit,
        float range,
        const CCBot& bot
    ) {
        auto&& enemies = bot.UnitInfo().getUnits(Players::Enemy);
        float maxPriority;
        std::optional<const Unit*> maxPriorityTarget = {};
        for (auto enemy : enemies) {
            float dist = Util::Dist(*enemy, *unit);
            // even if it is out of range it does not mean we shouldnt attack it
            if (dist > range) {
                break;
            }
            float threat = ThreatAnalyzer::getUnitTypeThreat(enemy->getType(), unit->getType());
            if (threat > maxPriority) {
                maxPriority = std::max(threat, maxPriority);
                maxPriorityTarget = enemy;
            } else if (threat == maxPriority) {
                if ((*maxPriorityTarget)->hpPercentage() > enemy->hpPercentage()) {
                    maxPriorityTarget = enemy;
                }
            }
        }
        return maxPriorityTarget;
    }

}