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
    validateFinish();
    if (m_completed) return;
    for (auto &unit : m_squad->units()) {
        if (!attackWhileMoving) {
            unit->move(targetPosition);
        } else {
            if (unit->getWeaponCooldown() == 0) {
                auto targetO = MicroUtil::findUnitWithHighestThreat(
                    unit,
                    m_bot
                );
                if (targetO.has_value()) {
                    unit->attackUnit(*targetO.value());
                } else {
                    unit->move(targetPosition);
                }
            } else {
                unit->move(targetPosition);
            }
        }
    }
}

void GroupAroundOrder::validateFinish() {
    auto center = Util::CalcCenter(m_squad->units());
    // what if center is unwalkable?
    float maxDist = 0;
    float distToTarget = std::numeric_limits<float>::max();

    for (auto &unit : m_squad->units()) {
        maxDist = std::max(maxDist, Util::Dist(unit, center));
        distToTarget = std::min(distToTarget, Util::Dist(unit, targetPosition));
    }

    // π * r^2 < units.size()
    if (distToTarget < 4 && maxDist * maxDist * 3.14 < m_squad->units().size()) {
        onEnd();
    }
}
