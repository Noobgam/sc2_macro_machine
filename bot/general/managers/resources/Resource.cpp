#include "Resource.h"
#include "general/CCBot.h"

Resource::Resource(const CCBot &bot, const Unit *unit, ResourceID id):
    m_unit(unit),
    m_bot(bot),
    m_id(id)
{
    m_lastUpdate = m_bot.getObservationId();
    if (unit->getType().isMineral()) {
        m_type = ResourceType::MINERAL;
    } else if (unit->getType().isGeyser()) {
        m_type = ResourceType::GEYSER;
    } else {
        BOT_ASSERT(false, "Resource is neither a mineral or geyser");
    }
}

void Resource::updateUnit(const Unit *unit) {
    m_unit = unit;
    m_lastUpdate = m_bot.getObservationId();
}

ResourceID Resource::getID() const {
    return m_id;
}

size_t Resource::getLastUpdate() const {
    return m_lastUpdate;
}

const Unit *Resource::getUnit() const {
    return m_unit;
}

ResourceType Resource::getResourceType() const {
    return m_type;
}

bool Resource::isVisible() const {
    return m_unit->getUnitPtr()->display_type == sc2::Unit::DisplayType::Visible;
}

CCPosition Resource::getPosition() const {
    return m_unit->getPosition();
}

int Resource::getResourceAmount() const {
    if (m_type == ResourceType::MINERAL) {
        return m_unit->getUnitPtr()->mineral_contents;
    } else {
        return m_unit->getUnitPtr()->vespene_contents;
    }
}
