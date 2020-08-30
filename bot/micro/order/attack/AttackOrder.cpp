#include "AttackOrder.h"

AttackOrder::AttackOrder(CCBot &bot, Squad* squad, CCPosition position) :
    Order(bot, squad),
    m_target_position(position) { }

void AttackOrder::onStart() {
    m_new_units.insert(m_new_units.end(), m_squad->units().begin(), m_squad->units().end());
}

void AttackOrder::onStep() {
    for (auto & unit : m_new_units) {
        unit->attackMove(m_target_position);
    }
}

void AttackOrder::onUnitAdded(const Unit *unit) {
    m_new_units.emplace_back(unit);
}

