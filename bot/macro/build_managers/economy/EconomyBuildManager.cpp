#include "EconomyBuildManager.h"
#include "GasBuildManager.h"

EconomyBuildManager::EconomyBuildManager(CCBot& bot)
        : m_bot(bot),
          m_worker_manager(WorkerBuildManager(bot)),
          m_gas_manager(GasBuildManager(bot)),
          m_expand_manager(ExpandBuildManager(bot))
{
}

std::optional<BuildOrderItem> EconomyBuildManager::getTopPriority() {
    auto workerBuildItem = m_worker_manager.getTopPriority();
    auto gasBuildItem = m_gas_manager.getTopPriority();
    if (gasBuildItem.has_value()) {
        if (workerBuildItem.has_value() && gasBuildItem.value() < workerBuildItem.value()) {
            return workerBuildItem;
        }
        return gasBuildItem;
    }
    return workerBuildItem;
}