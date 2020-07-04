#include "TechBuildManager.h"

#include "../../general/CCBot.h"

TechBuildManager::TechBuildManager(CCBot & bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> TechBuildManager::getTopPriority() {
    auto cyberneticsType = UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot);
    auto gatewayType = UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_bot);
    bool hasGate = m_bot.UnitInfo().getBuildingCount(Players::Self, gatewayType, UnitStatus::COMPLETED) > 0;
    int cyberneticsNumber = m_bot.UnitInfo().getBuildingCount(Players::Self, cyberneticsType, UnitStatus::TOTAL);
    if (!hasGate || cyberneticsNumber >= 1) {
        return {};
    }
    int priority = 8;
    BuildOrderItem item{ MetaType(cyberneticsType, m_bot), priority, false };
    return item;
}