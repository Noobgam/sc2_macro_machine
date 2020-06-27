#include "WorkerBuildManager.h"

WorkerBuildManager::WorkerBuildManager(CCBot& bot)
    : m_bot(bot)
{
}

std::optional<BuildOrderItem> WorkerBuildManager::getTopPriority()
{
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    auto nexusType = UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_bot);
    int freeNexuses = m_bot.UnitInfo().getBuildingCount(Players::Self, nexusType, UnitStatus::FREE);
    if (freeNexuses == 0) {
        return {};
    }
//    int totalWorkers = m_bot.Workers().getTotalWorkerCount();
//    int priority = (10648 - totalWorkers * totalWorkers * totalWorkers) * 10 / 10648;
    int priority = 12;
    BuildOrderItem item{ MetaType(probeType, m_bot), priority, false };
    return item;
}