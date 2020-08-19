#include <micro/util/MicroUtil.h>

#include <util/Util.h>

#include "GroupAroundOrder.h"

GroupAroundOrder::GroupAroundOrder(
    CCBot &bot,
    Squad *squad,
    CCPosition targetPosition,
    bool attackWhileMoving
)
  : Order(bot, squad)
  , attackWhileMoving(attackWhileMoving)
  , targetPosition(targetPosition)
{}

void GroupAroundOrder::onStep() {
    float radius = getCircleRadius();
    for (auto &unit : m_squad->units()) {
        if (attackWhileMoving && unit->getWeaponCooldown() < 0.1) {
            auto targetO = MicroUtil::findUnitWithHighestThreat(
                unit,
                m_bot
            );
            if (targetO.has_value()) {
                unit->attackUnit(*targetO.value());
                continue;
            }
        }
        if (Util::Dist(unit, targetPosition) > radius) {
            unit->move(targetPosition);
        }
    }
}

void GroupAroundOrder::draw() const {
    m_bot.Map().drawGroundCircle(targetPosition, getCircleRadius(), Colors::Purple);
    for (auto &unit : m_squad->units()) {
        m_bot.Map().drawLine(unit->getPosition(), targetPosition, Colors::Purple);
    }
}
float GroupAroundOrder::getCircleRadius() const {
    return sqrt(m_squad->units().size() / 2.28);
}
