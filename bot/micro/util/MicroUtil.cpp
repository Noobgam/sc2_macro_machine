#include <util/LogInfo.h>
#include "MicroUtil.h"

namespace MicroUtil {
    std::optional<const Unit*> findUnitWithHighestThreat(
        const Unit* unit,
        const std::vector<const Unit*>& enemies
    ) {
        // TODO: multiple weapons? Need to take both air and ground ranges into account
        return findUnitWithHighestThreat(
            unit,
            unit->getType().getAttackRange(),
            enemies
        );
    }

    std::optional<const Unit*> findUnitWithHighestThreat(
        const Unit* unit,
        float range,
        const std::vector<const Unit*>& enemies,
        bool filterPossible
    ) {
        float maxPriority = -1;
        std::optional<const Unit*> maxPriorityTarget = {};
        for (auto enemy : enemies) {
            if (!enemy->isValid()) {
                LOG_DEBUG << "Unit is invalid" << BOT_ENDL;
                continue;
            }
            if (enemy->getUnitPtr()->display_type != sc2::Unit::DisplayType::Visible) {
                continue;
            }
            if (filterPossible && !enemy->canAttack(unit)) {
                continue;
            }
            float dist = Util::Dist(*enemy, *unit) - enemy->getUnitPtr()->radius - unit->getUnitPtr()->radius;
            // even if it is out of range it does not mean we shouldnt attack it
            if (dist > range) {
                continue;
            }
            float threat = ThreatAnalyzer::getUnitTypeThreat(enemy->getType(), unit->getType());
            if (threat > maxPriority) {
                maxPriority = std::max(threat, maxPriority);
                maxPriorityTarget = enemy;
            } else if (threat == maxPriority) {
                // of units with same threat choose one that you'll be able to kill faster.
                int lowestHealth = (*maxPriorityTarget)->getHitPoints() + (*maxPriorityTarget)->getShields();
                int enemyHealth = enemy->getHitPoints() + enemy->getShields();
                if (
                    lowestHealth > enemyHealth
                        || (lowestHealth == enemyHealth && enemy->getID() < (*maxPriorityTarget)->getID())
                ) {
                    maxPriorityTarget = enemy;
                }
            }
        }
        return maxPriorityTarget;
    }

    std::vector<const Unit*> detectCloseTargets(CCPosition position, int range, const std::vector<const Unit*>& enemies) {
        std::vector<std::pair<float, const Unit *>> targets;
        for (auto enemy : enemies) {
            float dist = Util::Dist(*enemy, position);
            targets.emplace_back(dist, enemy);
        }
        std::sort(targets.begin(), targets.end());
        std::vector<const Unit *> closeTargets;
        for (auto x : targets) {
            if (x.first > range + 3) {
                break;
            }
            closeTargets.push_back(x.second);
        }
        return closeTargets;
    }
}