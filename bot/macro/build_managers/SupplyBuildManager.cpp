#include "SupplyBuildManager.h"

#include "../../general/CCBot.h"

SupplyBuildManager::SupplyBuildManager(CCBot & bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> SupplyBuildManager::getTopPriority() {
    auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
    int availableSupply = m_bot.GetMaxSupply() + getExpectedExtraSupply();
    int currentSupply = m_bot.GetCurrentSupply();
    int expectedConsumedSupply = getExpectedConsumedSupply();
    if (availableSupply < currentSupply) {
        return BuildOrderItem(MetaType(pylonType, m_bot), 1000, false);
    }
    if (currentSupply + expectedConsumedSupply > availableSupply) {
        return BuildOrderItem(MetaType(pylonType, m_bot), 12, false);
    }
    if (currentSupply + 2 * expectedConsumedSupply > availableSupply) {
        return BuildOrderItem(MetaType(pylonType, m_bot), 6, false);
    }
    if (availableSupply - expectedConsumedSupply - currentSupply < 10) {
        return BuildOrderItem(MetaType(pylonType, m_bot), 1, false);
    }
    return {};
}

int SupplyBuildManager::getExpectedConsumedSupply() const {
    auto nexusType = UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_bot);
    auto gateType = UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_bot);
    auto warpGateType = UnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE, m_bot);
    int nexuses = m_bot.UnitInfo().getBuildingCount(Players::Self, nexusType, UnitStatus::COMPLETED);
    int gates = m_bot.UnitInfo().getBuildingCount(Players::Self, gateType, UnitStatus::COMPLETED);
    int warpGates = m_bot.UnitInfo().getBuildingCount(Players::Self, warpGateType, UnitStatus::COMPLETED);
    return nexuses + gates * 2 + warpGates * 2;
}

int SupplyBuildManager::getExpectedExtraSupply() const {
    auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
    int pylonsInProgress = m_bot.UnitInfo().getBuildingCount(Players::Self, pylonType, UnitStatus::IN_PROGRESS);
    return pylonsInProgress * 8;
}