#include "GasBuildManager.h"
#include "../../../general/CCBot.h"

GasBuildManager::GasBuildManager(CCBot& bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> GasBuildManager::getTopPriority() {
//    TODO Rich gas
    auto assimilatorType = UnitType(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, m_bot);
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    auto nexusType = UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_bot);
    int assimilatorsCount = m_bot.UnitInfo().getBuildingCount(Players::Self, assimilatorType, UnitStatus::TOTAL);
    int nexusCount = m_bot.UnitInfo().getBuildingCount(Players::Self, nexusType, UnitStatus::TOTAL);
    int probeCount = m_bot.UnitInfo().getUnitTypeCount(Players::Self, probeType);
    int geysers = 0;
    for (const auto& location : m_bot.Bases().getBaseLocations()) {
        if (location->isOccupiedByPlayer(Players::Self)) {
            geysers += location->getGeysers().size();
        }
    }
    if (geysers - assimilatorsCount > 0 && probeCount >= 16 + assimilatorsCount * 3) {
        int targetAssimilatorCount = (probeCount - nexusCount * 16) / 3;
        int priority = 7 + targetAssimilatorCount - assimilatorsCount;
        return BuildOrderItem(MetaType(assimilatorType, m_bot), priority, false);
    }
    return {};
}
