#include "EconomyBuildManager.h"
#include "../../../general/CCBot.h"

EconomyBuildManager::EconomyBuildManager(CCBot& bot)
        : BuildManager(bot),
          m_worker_manager(WorkerBuildManager(bot)),
          m_gas_manager(GasBuildManager(bot)),
          m_expand_manager(ExpandBuildManager(bot))
{ }

std::optional<BuildOrderItem> EconomyBuildManager::getTopPriority() {
    std::vector<BuildOrderItem> items;
    auto workerBuildItem = m_worker_manager.getTopPriority();
    if (workerBuildItem.has_value()){
        items.emplace_back(workerBuildItem.value());
    }
    auto gasBuildItem = m_gas_manager.getTopPriority();
    if (gasBuildItem.has_value()){
        items.emplace_back(gasBuildItem.value());
    }
    auto expandBuildItem = m_expand_manager.getTopPriority();
    if (expandBuildItem.has_value()){
        items.emplace_back(expandBuildItem.value());
    }
    auto item_ptr = std::max_element(items.begin(), items.end());
    if (item_ptr == items.end()) {
        return {};
    }
    return *item_ptr;
}