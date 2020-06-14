#include "ProductionManager.h"

ProductionManager::ProductionManager(CCBot& bot)
    : m_bot(bot)
{
}

std::pair<float, float> ProductionManager::approximateIncome() {
    // TODO: take oversaturation into account
    auto& workers = m_bot.Workers();
    // Data extracted from graph https://liquipedia.net/starcraft2/Mining_Minerals#Chart_Realization.2FMethodology
    int mineralIncome = workers.getNumMineralWorkers() * 12.5;

    // Ideal mining is 2.82 /s https://liquipedia.net/starcraft2/Resources#Vespene_Gas_2
    int gasIncome = workers.getNumGasWorkers() * 2.7;
    return { mineralIncome, gasIncome };
}


std::optional<BuildOrderItem> ProductionManager::getTopPriority()
{
    auto gatewayType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY);
    auto warpGateType = m_bot.getUnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE);
    auto& units = m_bot.GetUnits();
    int gateWayCount = 0;
    int warpGateCount = 0;
    for (const auto& unit : m_bot.GetUnits()) {
        if (unit.isValid() && (unit.getType() == gatewayType || unit.getType() == warpGateType) && unit.getPlayer() == Players::Self) {
            // this is a heuristic, there are couple of frames lost between placeholder and construction
            //  so workers will build more than one building.
            if (unit.isAlive() || unit.isBeingConstructed() || unit.getUnitPtr()->display_type == sc2::Unit::DisplayType::Placeholder) {
                if (unit.getType() == gatewayType) {
                    ++gateWayCount;
                }
                else {
                    ++warpGateCount;
                }
            }
        }
    }
    // production time: https://liquipedia.net/starcraft2/Warp_Gate_(Legacy_of_the_Void)#Description
    double potentialAdeptCount = gateWayCount * (1.0 / 27) + warpGateCount * (1.0 / 20);
    // ADEPT_COST = { 100, 25 }
    // ZEALOT = { 100, 0 }
    std::pair<float, float> production = { 100 * potentialAdeptCount, 0 * potentialAdeptCount };
    std::pair<float, float> income = approximateIncome();
    if (income.first < production.first || income.second < production.second) {
        return {};
    }
    BuildOrderItem defaultGatePriority{ { gatewayType, m_bot }, 5, false };
    if (potentialAdeptCount == 0) {
        return defaultGatePriority;
    }

    // TODO: better formula?
    return defaultGatePriority;
}