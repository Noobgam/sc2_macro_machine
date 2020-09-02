#include <util/Util.h>
#include <general/ThreatAnalyzer.h>
#include <micro/util/MicroUtil.h>
#include <micro/order/commands/Commands.h>
#include <util/LogInfo.h>

#include "AttackWithKiting.h"

AttackWithKiting::AttackWithKiting(CCBot &bot, Squad *squad, CCPosition position)
    : Order(bot, squad)
    , m_target_position(position) { }

void AttackWithKiting::onStart() { }

void AttackWithKiting::onStep() {
    for (auto unit : m_squad->units()) {
        handleOneUnit(unit);
    }
}

void AttackWithKiting::onUnitAdded(const Unit *unit) { }

void AttackWithKiting::handleOneUnit(const Unit *unit) {
    auto&& enemies = m_bot.UnitInfo().getUnits(Players::Enemy);
    auto it = endangered.find(unit->getID());
    bool inDanger;
    if (it != endangered.end()) {
        if (unit->shieldPercentage() > 0.5) {
            endangered.erase(it);
            inDanger = false;
        } else {
            inDanger = true;
        }
    } else {
        inDanger = unit->hpPercentage() < 0.4 && unit->getShields() == 0;
        if (inDanger) {
            endangered.insert(unit->getID());
        }
    }
    if (inDanger) {
        if (!Commands::kiteBack(m_bot, unit, enemies)) {
            attackMoveToMainTarget(unit);
        }
    } else {
        // check for situation here
        auto targetO = MicroUtil::findUnitWithHighestThreat(unit, enemies);
        if (targetO.has_value()) {
            LOG_DEBUG << "PRIO " << targetO.value()->getType().getName() << BOT_ENDL;
        }
        if (targetO.has_value() && (targetO.value()->getType().isBuilding() || targetO.value()->getType().isWorker())) {
            LOG_DEBUG << "BUILDING IS TOP PRIO" << BOT_ENDL;
            if (!Commands::pushForward(m_bot, unit, enemies, targetO.value()->getPosition())) {
                attackMoveToMainTarget(unit);
            }
        } else {
            LOG_DEBUG << "KITE!" << BOT_ENDL;
            if (!Commands::kiteBack(m_bot, unit, enemies)) {
                LOG_DEBUG << "NOTHING!" << BOT_ENDL;
                attackMoveToMainTarget(unit);
            }
        }
    }
}

void AttackWithKiting::attackMoveToMainTarget(const Unit* unit) {
    unit->attackMove(m_target_position);
    if (Util::Dist(m_target_position, unit->getPosition()) < 2) {
        onEnd();
    }
}