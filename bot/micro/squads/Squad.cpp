#include "Squad.h"

Squad::Squad(SquadID id): m_id(id) {

}

void Squad::addUnits(const std::set<Unit*> & units) {
    m_units.insert(units.begin(), units.end());
}

void Squad::removeUnits(const std::set<Unit*> & units) {
    m_units.erase(units.begin(), units.end());
}

void Squad::clear() {
    m_units.clear();
}

const std::set<Unit*> & Squad::units() const {
    return m_units;
}

bool Squad::isEmpty() const {
    return m_units.empty();
}

SquadID Squad::getId() const {
    return m_id;
}
