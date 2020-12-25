#include <util/LogInfo.h>
#include "SupplyBuildManager.h"

#include "../../general/CCBot.h"

SupplyBuildManager::SupplyBuildManager(CCBot & bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> SupplyBuildManager::getTopPriority() {
    auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
    int availableSupply = m_bot.GetMaxSupply() + getExpectedExtraSupply();
    int currentSupply = m_bot.GetCurrentSupply();
    int expectedConsumedSupply = getExpectedConsumedSupply();
    LOG_INFO << currentSupply << " : " << availableSupply << " : " << expectedConsumedSupply << BOT_ENDL;
    if (availableSupply <= currentSupply) {
        return BuildOrderItem(MetaType(pylonType, m_bot), 1000, false);
    }
    if (currentSupply + expectedConsumedSupply > availableSupply) {
        return BuildOrderItem(MetaType(pylonType, m_bot), 12, false);
    }
    if (currentSupply + 2 * expectedConsumedSupply > availableSupply) {
        return BuildOrderItem(MetaType(pylonType, m_bot), 6, false);
    }
    if (availableSupply - expectedConsumedSupply - currentSupply < 10) {

        auto gatewayType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY);
        auto warpGateType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE);
        int gateWayCount = m_bot.UnitInfo().getBuildingCount(Players::Self, gatewayType, UnitStatus::TOTAL);
        int warpGateCount = m_bot.UnitInfo().getBuildingCount(Players::Self, warpGateType, UnitStatus::TOTAL);
        if (gateWayCount + warpGateCount > 0) {
            // if there are no gates (e.g. cannon rushing) there is pretty much no way to waste as much supply
            return BuildOrderItem(MetaType(pylonType, m_bot), 1, false);
        }
    }
    return {};
}

int SupplyBuildManager::getExpectedConsumedSupply() const {
    auto nexusType = UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_bot);
    auto gateType = UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_bot);
    auto warpGateType = UnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE, m_bot);
    auto cyberneticsType = UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot);
    int nexuses = m_bot.UnitInfo().getBuildingCount(Players::Self, nexusType, UnitStatus::COMPLETED);
    int gates = m_bot.UnitInfo().getBuildingCount(Players::Self, gateType, UnitStatus::COMPLETED);
    int warpGates = m_bot.UnitInfo().getBuildingCount(Players::Self, warpGateType, UnitStatus::COMPLETED);
    int cybernetics = m_bot.UnitInfo().getBuildingCount(Players::Self, cyberneticsType, UnitStatus::COMPLETED);
    int coef = cybernetics >= 1 ? 2 : 0;
    return nexuses + gates * coef + warpGates * coef;
}

int SupplyBuildManager::getExpectedExtraSupply() const {
    auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
    int pylonsInProgress = m_bot.UnitInfo().getBuildingCount(Players::Self, pylonType, UnitStatus::IN_PROGRESS);
    int res = pylonsInProgress * 8;
    for (const auto& nexus : m_bot.UnitInfo().getUnits(Players::Self, sc2::UNIT_TYPEID::PROTOSS_NEXUS)) {
        if (!nexus->isCompleted() && nexus->getBuildPercentage() > 0.75) {
            res += 15;
        }
    }
    return res;
}