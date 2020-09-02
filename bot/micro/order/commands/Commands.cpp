#include <util/Util.h>
#include <micro/util/MicroUtil.h>
#include <util/LogInfo.h>
#include "Commands.h"

#include "general/CCBot.h"

namespace Commands {
    bool retreat(const CCBot &bot, const Unit *unit, std::vector<const Unit *> enemies) {
        float range = unit->getType().getAttackRange();
        const auto closeTargets = MicroUtil::detectCloseTargets(unit->getPosition(), range + 3,enemies);
        // if on cooldown - run away from targets who are too close
        if (closeTargets.empty()) {
            return false;
        }
        auto center = Util::CalcCenter(closeTargets);
        auto vec_to_center = center - unit->getPosition();
        auto backward_direction = Util::NormalizeVector(vec_to_center * -2);
        CCPosition walkable = bot.Map().findClosestWalkablePosition(unit->getPosition() + backward_direction);
        unit->move(walkable);
        return true;
    }

    bool attackTargetWithHighestThreat(const CCBot &bot, const Unit *unit, std::vector<const Unit *> enemies) {
        auto targetO = MicroUtil::findUnitWithHighestThreat(unit, enemies);
        if (!targetO.has_value() || !unit->canAttack(targetO.value())) {
            return false;
        } else {
            unit->attackUnit(*targetO.value());
            LOG_DEBUG << "ATTACK! " << targetO.value()->getType().getName() << BOT_ENDL;
            return true;
        }
    }

    bool kiteBack(const CCBot &bot, const Unit *unit, std::vector<const Unit *> enemies) {
        bool weaponOnCooldown = unit->getWeaponCooldown() > 0.1;
        if (!weaponOnCooldown) {
            if (!Commands::attackTargetWithHighestThreat(bot, unit, enemies)) {
                return false;
            }
        } else {
            if (!Commands::retreat(bot, unit, enemies)) {
                return false;
            }
        }
        return true;
    }

    bool pushForward(const CCBot &bot, const Unit *unit, std::vector<const Unit *> enemies) {
        float range = unit->getType().getAttackRange();
        auto targetO = MicroUtil::findUnitWithHighestThreat(unit,range + 3, enemies);
        if (!targetO.has_value()) {
            return false;
        } else {
            return pushForward(bot, unit, enemies, targetO.value()->getPosition());
        }
    }

    bool pushForward(const CCBot &bot, const Unit *unit, std::vector<const Unit *> enemies, CCPosition position) {
        bool weaponOnCooldown = unit->getWeaponCooldown() > 0.1;
        if (!weaponOnCooldown) {
            if (!Commands::attackTargetWithHighestThreat(bot, unit, enemies)) {
                return false;
            }
        } else {
            unit->move(position);
            return true;
        }
        return true;
    }
}