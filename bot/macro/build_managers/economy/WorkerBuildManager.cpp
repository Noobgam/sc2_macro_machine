#include "WorkerBuildManager.h"

#include "../../../general/CCBot.h"

WorkerBuildManager::WorkerBuildManager(CCBot& bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> WorkerBuildManager::getTopPriority() {
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    auto nexusType = UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_bot);
    int freeNexuses = m_bot.UnitInfo().getBuildingCount(Players::Self, nexusType, UnitStatus::FREE);
    if (freeNexuses == 0) {
        return {};
    }
    int totalWorkers = m_bot.UnitInfo().getUnitTypeCount(Players::Self, probeType, false);
    if (totalWorkers >= 80) {
        return {};
    }
//    int priority = (10648 - totalWorkers * totalWorkers * totalWorkers) * 10 / 10648;
    int priority = 13;
    BuildOrderItem item{ MetaType(probeType, m_bot), priority, false };
    return item;
}
