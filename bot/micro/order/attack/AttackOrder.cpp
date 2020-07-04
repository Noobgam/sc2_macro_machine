#include "AttackOrder.h"

AttackOrder::AttackOrder(CCBot &bot, Squad* squad, CCPosition position) :
    Order(bot, squad),
    m_target_position(position) { }

void AttackOrder::onStep() {
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    for (auto & unit : m_squad->units()) {
        if (unit->getType() != probeType) {
            unit->attackMove(m_target_position);
        }
    }
}
