#include "AttackOrder.h"

AttackOrder::AttackOrder(CCBot &bot, Squad* squad, CCPosition position) :
    Order(bot, squad),
    m_target_position(position) { }

void AttackOrder::onStep() {
    for (auto & unit : m_squad->units()) {
        unit->attackMove(m_target_position);
    }
}
