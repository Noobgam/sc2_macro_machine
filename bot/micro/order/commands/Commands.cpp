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
            // this move command avoids deceleration of units, units walk towards target until they are in range.
            // although this must be optimized for units with more than one weapon
            // e.g. tempest
            //
            // although the most abusable unit for this mechanic is a viking.
            auto dist = Util::Dist(*unit, *targetO.value());
            if (unit->getType().getAttackRange() + 0.1 < dist) {
                unit->move(*targetO.value());
            } else {
                unit->attackUnit(*targetO.value());
            }
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

    bool destroyUnits(const CCBot &bot, const Unit *unit, std::vector<const Unit *> enemies) {
        if (enemies.empty()) {
            return false;
        }
        auto targetO = MicroUtil::findUnitWithHighestThreat(unit, 10000, enemies);
        if (!targetO.has_value()) {
            return false;
        } else {
            bool weaponOnCooldown = unit->getWeaponCooldown() > 0.1;
            if (!weaponOnCooldown) {
                unit->attackUnit(*targetO.value());
                return true;
            } else {
                unit->move(targetO.value()->getPosition());
                return true;
            }
        }
        return true;
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