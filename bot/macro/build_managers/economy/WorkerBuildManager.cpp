#include "WorkerBuildManager.h"

WorkerBuildManager::WorkerBuildManager(CCBot& bot)
    : m_bot(bot)
{
}

std::optional<BuildOrderItem> WorkerBuildManager::getTopPriority()
{
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    int freeNexuses = 0;
    for (auto& unit : m_bot.GetUnits()) {
        if (!unit.isValid()) continue;
        if (!unit.isAlive()) continue;
        if (!unit.isOfType(sc2::UNIT_TYPEID::PROTOSS_NEXUS)) continue;
        if (unit.isTraining()) continue;
        ++freeNexuses;
    }   
    if (freeNexuses == 0) {
        return {};
    }
//    int totalWorkers = m_bot.Workers().getTotalWorkerCount();
//    int priority = (10648 - totalWorkers * totalWorkers * totalWorkers) * 10 / 10648;
    int priority = 12;
    BuildOrderItem item{ MetaType(probeType, m_bot), priority, false };
    return item;
}
