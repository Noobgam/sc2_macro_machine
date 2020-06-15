#include "GasBuildManager.h"

GasBuildManager::GasBuildManager(CCBot& bot)
        : m_bot(bot)
{
}

std::optional<BuildOrderItem> GasBuildManager::getTopPriority()
{
//    TODO Rich gas
    auto assimilatorType = UnitType(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, m_bot);
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    int totalWorkers = m_bot.Workers().getTotalWorkerCount();
    int assimilators = 0;
    int probes = 0;
    for (auto unit : m_bot.GetUnits() ) {
        if (unit.isValid() && unit.getPlayer() == Players::Self) {
            if (unit.getType() == assimilatorType) {
                if (unit.isAlive() || unit.getUnitPtr()->display_type == sc2::Unit::DisplayType::Placeholder) {
                    assimilators++;
                }
            }
            if (unit.getType() == probeType) {
                probes++;
            }
        }
    }
    int geysers = 0;
    for (const auto& location : m_bot.Bases().getBaseLocations()) {
        if (location->isOccupiedByPlayer(Players::Self)) {
            geysers += location->getGeysers().size();
        }
    }
    if (geysers - assimilators > 0 && probes >= 16) {
        int priority = 11 + (probes - 16) / 3;
        return BuildOrderItem(MetaType(assimilatorType, m_bot), priority, false);
    }
    return {};
}
