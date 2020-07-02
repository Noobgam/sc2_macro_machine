#include "AttackOrder.h"

AttackOrder::AttackOrder(CCBot &bot, CCPosition position) : Order(bot), m_target_position(position) { }

void AttackOrder::onStart(Squad* squad) { }

void AttackOrder::onStep(Squad* squad) {
    auto probeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_bot);
    for (auto & unit : squad->units()) {
        if (unit->getType() != probeType) {
            unit->attackMove(m_target_position);
        }
    }
}
