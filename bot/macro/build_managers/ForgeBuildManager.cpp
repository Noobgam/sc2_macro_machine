#include "ForgeBuildManager.h"

#include "../../general/CCBot.h"

ForgeBuildManager::ForgeBuildManager(CCBot & bot) : BuildManager(bot) { }

std::optional<BuildOrderItem> ForgeBuildManager::getTopPriority() {
    auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
    auto forgeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot);
    bool hasPylon = m_bot.UnitInfo().getBuildingCount(Players::Self, pylonType, UnitStatus::COMPLETED) > 0;
    int forgeNumber = m_bot.UnitInfo().getBuildingCount(Players::Self, forgeType, UnitStatus::TOTAL);
    if (forgeNumber >= 1) {
        return {};
    }
    if (!hasPylon) {
        return {};
    }
    return BuildOrderItem (MetaType(forgeType, m_bot), 20, false );
}