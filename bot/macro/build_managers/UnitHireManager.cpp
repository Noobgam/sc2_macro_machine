#include "UnitHireManager.h"

#include "../../general/CCBot.h"

UnitHireManager::UnitHireManager(CCBot& bot) : BuildManager(bot) { }


std::optional<BuildOrderItem> UnitHireManager::getTopPriority() {
    auto gatewayType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY);
    auto warpGateType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE);
    auto zealot = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_ZEALOT);
    int freeFateWayCount = m_bot.UnitInfo().getBuildingCount(Players::Self, gatewayType, UnitStatus::FREE);
    int freeWarpGateCount = m_bot.UnitInfo().getBuildingCount(Players::Self, warpGateType, UnitStatus::FREE);
    if (freeFateWayCount + freeWarpGateCount == 0) {
        return {};
    }
    return { { {zealot, m_bot}, 7, false } };
}