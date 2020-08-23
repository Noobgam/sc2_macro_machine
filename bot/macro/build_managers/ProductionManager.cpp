#include "ProductionManager.h"
#include "../../general/CCBot.h"

ProductionManager::ProductionManager(CCBot& bot) : BuildManager(bot) { }

std::pair<double, double> ProductionManager::approximateIncome() {
    // TODO: take oversaturation into account
//    auto& workers = m_bot.Workers();
    // Data extracted from graph https://liquipedia.net/starcraft2/Mining_Minerals#Chart_Realization.2FMethodology
    double mineralIncome = m_bot.getManagers().getEconomyManager().getMineralIncome();
    double vespeneIncome = m_bot.getManagers().getEconomyManager().getVespeneIncome();

    // Ideal mining is 2.82 /s https://liquipedia.net/starcraft2/Resources#Vespene_Gas_2
//    int gasIncome = workers.getNumGasWorkers() * 2.7;

    return { mineralIncome, vespeneIncome };
}


std::optional<BuildOrderItem> ProductionManager::getTopPriority() {
    auto pylonType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON);
    int pylonCount = m_bot.UnitInfo().getBuildingCount(Players::Self, pylonType, UnitStatus::COMPLETED);
    if (pylonCount == 0) {
        return {};
    }
    auto gatewayType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY);
    auto warpGateType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE);
    int gateWayCount = m_bot.UnitInfo().getBuildingCount(Players::Self, gatewayType, UnitStatus::TOTAL);
    int warpGateCount = m_bot.UnitInfo().getBuildingCount(Players::Self, warpGateType, UnitStatus::TOTAL);
    int activeGatesCount = gateWayCount + warpGateCount;
    // production time: https://liquipedia.net/starcraft2/Warp_Gate_(Legacy_of_the_Void)#Description
    double potentialAdeptCount = gateWayCount * (1.0 / 27) + warpGateCount * (1.0 / 20);
    // ADEPT_COST = { 100, 25 }
    // ZEALOT = { 100, 0 }
    std::pair<float, float> production = { 100 * potentialAdeptCount, 0 * potentialAdeptCount };
    std::pair<float, float> income = approximateIncome();
    int requiredGatesCountMineral = income.first / (150.0 / 42);
    int requiredGatesCountVespene = income.second / (50.0 / 42);
    int requiredGatesCount = std::min(requiredGatesCountMineral, requiredGatesCountVespene);
    int additionalGateCount = requiredGatesCount - activeGatesCount;
    if (additionalGateCount <= 0) {
        return {};
    }
    return BuildOrderItem({ gatewayType, m_bot }, 2 + additionalGateCount, false );
}