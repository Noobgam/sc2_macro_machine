#include "Squad.h"

Squad::Squad(SquadID id): m_id(id) { }

void Squad::addUnits(const std::set<const Unit*> & units) {
    for (auto unit : units) {
        m_units.insert(unit);
    }
}

void Squad::removeUnits(const std::set<const Unit*> & units) {
    for (auto unit : units) {
        m_units.erase(unit);
    }
}

void Squad::clear() {
    m_units.clear();
}

const std::set<const Unit*> & Squad::units() const {
    return m_units;
}

bool Squad::isEmpty() const {
    return m_units.empty();
}

SquadID Squad::getId() const {
    return m_id;
}
