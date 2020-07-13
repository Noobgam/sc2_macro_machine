#include "Mineral.h"
#include "general/CCBot.h"

Mineral::Mineral(CCBot &bot, const Unit *unit, MineralID id):
    m_unit(unit),
    m_bot(&bot),
    m_unitType(unit->getType()),
    m_id(id)
{
    m_lastUpdate = m_bot->getObservationId();
}

void Mineral::updateUnit(const Unit *unit) {
    m_unit = unit;
    m_lastUpdate = m_bot->getObservationId();
}

MineralID Mineral::getID() const {
    return m_id;
}

size_t Mineral::getLastUpdate() const {
    return m_lastUpdate;
}

const Unit *Mineral::getUnit() const {
    return m_unit;
}

bool Mineral::isVisible() const {
    return m_unit->getUnitPtr()->display_type == sc2::Unit::DisplayType::Visible;
}

CCPosition Mineral::getPosition() const {
    return m_unit->getPosition();
}

int Mineral::getMineralsAmount() const {
    if (!isVisible()) {
        // Unknown value?
        return 0;
    }
    return m_unit->getUnitPtr()->mineral_contents;
}
