#include "SupplyBuildManager.h"

SupplyBuildManager::SupplyBuildManager(CCBot & bot)
    : m_bot(bot)
{
}

std::optional<BuildOrderItem> SupplyBuildManager::getTopPriority()
{
    auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
    int allSupply = m_bot.GetMaxSupply() + getExpectedExtraSupply();
    int priority;
    if (allSupply <= m_bot.GetCurrentSupply()) {
        priority = 1000;
    } else {
        priority = 10 / (allSupply - m_bot.GetCurrentSupply());
    }
    BuildOrderItem item(MetaType(pylonType, m_bot), priority, false);
    return item;
}

int SupplyBuildManager::getExpectedExtraSupply()
{
    auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
    int pylonsInProgress = m_bot.UnitInfo().getBuildingCount(Players::Self, pylonType, UnitStatus::IN_PROGRESS);
    return pylonsInProgress * 8;
}