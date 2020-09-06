#include <util/Util.h>
#include <micro/util/MicroUtil.h>
#include <micro/order/commands/Commands.h>
#include "ProtectPointOrder.h"

#include "general/CCBot.h"

ProtectPointOrder::ProtectPointOrder(CCBot &bot, Squad *squad, CCPosition point)
    : Order(bot, squad),
      m_point(point)
{ }

void ProtectPointOrder::onStep() {
    const auto radius = getCircleRadius();
    const auto& enemies = m_bot.UnitInfo().getUnits(Players::Enemy);
    const auto& closeToPointEnemies = MicroUtil::detectCloseTargets(m_point, m_PROTECT_RADIUS, enemies);
    for (auto& unit : m_squad->units()) {
        const auto dist = Util::Dist(unit, m_point);
        if (!Commands::destroyUnits(m_bot, unit, closeToPointEnemies)) {
            if (dist > radius) {
                if (!Commands::pushForward(m_bot, unit, enemies, m_point)) {
                    unit->attackMove(m_point);
                }
            }
        }
    }
    draw();
}

void ProtectPointOrder::draw() const {
    m_bot.Map().drawGroundCircle(m_point, getCircleRadius(), Colors::Purple);
    for (auto &unit : m_squad->units()) {
        m_bot.Map().drawLine(unit->getPosition(), m_point, Colors::Purple);
    }
}

float ProtectPointOrder::getCircleRadius() const {
    return sqrt(m_squad->units().size() / 3.14) * 1.9;
}