#include "TechBuildManager.h"

#include "../../general/CCBot.h"

TechBuildManager::TechBuildManager(CCBot & bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> TechBuildManager::getTopPriority() {
    auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
    auto cyberneticsType = UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot);
    auto gatewayType = UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_bot);
    auto warpGateType = UnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE, m_bot);
    bool hasPylon = m_bot.UnitInfo().getBuildingCount(Players::Self, pylonType, UnitStatus::COMPLETED) > 0;
    bool hasGate = (
        m_bot.UnitInfo().getBuildingCount(Players::Self, gatewayType, UnitStatus::COMPLETED) +
        m_bot.UnitInfo().getBuildingCount(Players::Self, warpGateType, UnitStatus::COMPLETED)
    ) > 0;
    int cyberneticsNumber = m_bot.UnitInfo().getBuildingCount(Players::Self, cyberneticsType, UnitStatus::TOTAL);
    if (!hasPylon) {
        return {};
    }
    if (!hasGate) {
        if (m_bot.UnitInfo().getBuildingCount(Players::Self, gatewayType, UnitStatus::TOTAL) > 0) {
            return {};
        }
        return BuildOrderItem(MetaType(gatewayType, m_bot), 10, false);
    }
    if (cyberneticsNumber >= 1) {
        return {};
    }
    return BuildOrderItem (MetaType(cyberneticsType, m_bot), 15, false );
}