#include "GasBuildManager.h"
#include "../../../general/CCBot.h"

GasBuildManager::GasBuildManager(CCBot& bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> GasBuildManager::getTopPriority() {
//    TODO Rich gas
    auto assimilatorType = UnitType(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, m_bot);
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    auto nexusType = UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_bot);
    int assimilatorsCount = m_bot.UnitInfo().getBuildingCount(Players::Self, assimilatorType, UnitStatus::TOTAL);
    int nexusCount = m_bot.UnitInfo().getBuildingCount(Players::Self, nexusType, UnitStatus::COMPLETED);
    int probeCount = m_bot.UnitInfo().getUnitTypeCount(Players::Self, probeType);
    int geysers = 0;
    for (const auto& base : m_bot.getManagers().getBasesManager().getCompletedBases()) {
        geysers += base->getBaseLocation()->getGeysers().size();
    }
    if (geysers - assimilatorsCount > 0 && probeCount >= 16 * nexusCount + assimilatorsCount * 3 - nexusCount) {
        int targetAssimilatorCount = (probeCount - nexusCount * 16) / 3;
        int priority = 7 + targetAssimilatorCount - assimilatorsCount;
        return BuildOrderItem(MetaType(assimilatorType, m_bot), priority, false);
    }
    return {};
}
