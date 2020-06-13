#include "UnitHireManager.h"

UnitHireManager::UnitHireManager(CCBot& bot)
    : m_bot(bot)
{
}


std::optional<BuildOrderItem> UnitHireManager::getTopPriority()
{
    auto gatewayType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY);
    auto warpGateType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE);
    auto zealot = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_ZEALOT);
    auto& units = m_bot.GetUnits();
    int gateWayCount = 0;
    int warpGateCount = 0;
    for (const auto& unit : m_bot.GetUnits()) {
        if (unit.isValid() && (unit.getType() == gatewayType || unit.getType() == warpGateType) && unit.getPlayer() == Players::Self) {
            // this is a heuristic, there are couple of frames lost between placeholder and construction
            //  so workers will build more than one building.
            if (unit.isAlive() && !unit.isBeingConstructed() && !unit.isTraining()) {
                if (unit.getType() == gatewayType) {
                    ++gateWayCount;
                }
                else {
                    ++warpGateCount;
                }
            }
        }
    }
    if (gateWayCount + warpGateCount == 0) {
        return {};
    }
    return { { {zealot, m_bot}, 7, false } };
}