#include "ExpandBuildManager.h"

ExpandBuildManager::ExpandBuildManager(CCBot &bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> ExpandBuildManager::getTopPriority() {
    return std::optional<BuildOrderItem>();
}
