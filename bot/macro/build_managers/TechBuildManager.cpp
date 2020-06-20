#include "TechBuildManager.h"

TechBuildManager::TechBuildManager(CCBot & bot)
        : m_bot(bot)
{ }

std::optional<BuildOrderItem> TechBuildManager::getTopPriority()
{
    auto cyberneticsType = UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot);
    auto gatewayType = UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_bot);
    bool hasGate = false;
    int cyberneticsNumber = 0;
    for (auto unit : m_bot.GetUnits() ) {
        if (unit.isValid() && unit.getPlayer() == Players::Self) {
            if (unit.getType() == gatewayType && unit.isAlive() && !unit.isBeingConstructed()) {
                hasGate = true;
            }
            if (unit.getType() == cyberneticsType) {
                if (unit.isAlive() || unit.getUnitPtr()->display_type == sc2::Unit::DisplayType::Placeholder) {
                    cyberneticsNumber++;
                }
            }
        }
    }
    if (!hasGate || cyberneticsNumber >= 1) {
        return {};
    }
    int priority = 8;
    BuildOrderItem item{ MetaType(cyberneticsType, m_bot), priority, false };
    return item;
}