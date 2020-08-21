#include "ExpandBuildManager.h"
#include "general/CCBot.h"

ExpandBuildManager::ExpandBuildManager(CCBot &bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> ExpandBuildManager::getTopPriority() {
    const auto& economyManager = m_bot.getManagers().getEconomyManager();
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    auto nexusType = UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_bot);
    int bases = m_bot.getManagers().getBasesManager().getBases().size();
    if (bases >= m_bot.Bases().getBaseLocations().size() / 2) {
        return {};
    }
    int requiredProbes = economyManager.getMineralWorkersPositions() + economyManager.getVespeneWorkersPositions();
    int probes = m_bot.UnitInfo().getUnitTypeCount(Players::Self, probeType);
    int priority = probes - requiredProbes + 12;
    if (priority < 0) {
        return {};
    }
    BuildOrderItem item{ MetaType(nexusType, m_bot), priority, false };
    return item;
}
