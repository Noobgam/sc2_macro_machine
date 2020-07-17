#include "AttackOrder.h"

AttackOrder::AttackOrder(CCBot &bot, Squad* squad, CCPosition position) :
    Order(bot, squad),
    m_target_position(position) { }

void AttackOrder::onStart() {
    for (auto & unit : m_squad->units()) {
        unit->attackMove(m_target_position);
    }
}

void AttackOrder::onStep() { }

void AttackOrder::onUnitAdded(const Unit *unit) {
    unit->attackMove(m_target_position);
}

