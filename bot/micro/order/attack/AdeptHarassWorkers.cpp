#include "AdeptHarassWorkers.h"
#include <general/CCBot.h>

#include <micro/order/commands/Commands.h>
#include <boost/container/flat_set.hpp>
#include <util/Util.h>
#include <util/LogInfo.h>

using boost::container::flat_set;

void AdeptHarassWorkers::onStep() {
    // pretend like all adepts are always together.
    // undefined behaviour when adepts are added dynamically to the squad
    bool allCanShade = true;
    int currentFrameId = m_bot.GetCurrentFrame();
    for (auto&& x : m_squad->units()) {
        if (!x->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPT)) {
            continue;
        }
        auto it = lastShadeCast.find(x);
        if (it == lastShadeCast.end()) {
            continue;
        }
        if (currentFrameId - it->second < AdeptHarassWorkers::SHADE_COOLDOWN) {
            allCanShade = false;
            break;
        } else {
            lastShadeCast.erase(it);
        }
    }
    LOG_DEBUG << "Frame id: " << currentFrameId << BOT_ENDL;
    if (allCanShade && !adeptsAndShades.empty()) {
        // TODO: middle of resources is not a good spot for adepts.
        CCPosition targetPosition = m_bot.Bases().getBaseLocation(currentTargetBase)->getPosition();
        LOG_DEBUG << "Shading!" << currentFrameId << BOT_ENDL;
        for (auto&& adept : m_squad->units()) {
            if (!adept->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPT)) {
                continue;
            }
            adept->castAbility(sc2::ABILITY_ID::EFFECT_ADEPTPHASESHIFT, targetPosition);
            lastShadeCast[adept] = currentFrameId;
        }
        return;
    }
    // first move shades since it is easier
    for (auto&& x : m_squad->units()) {
        if (x->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT)) {
            x->move(shadeTarget);
        }
    }
    // calculate what can adept directly hit, if they cant hit any workers they stay in the target spot.
    flat_map<const Unit*, std::vector<const Unit*>> candidates;
    for (auto&& unit : m_bot.UnitInfo().getUnits(Players::Enemy)) {
        if (unit->isValid() && unit->isAlive() && unit->getType().isWorker()) {
            for (auto&& lr : adeptsAndShades) {
                auto&& adept = lr.first;
                auto dist = Util::Dist(*unit, *adept);
                if (dist < adept->getType().getAttackRange() + .1) {
                    candidates[unit].push_back(adept);
                }
            }
        }
    }
    flat_map<const Unit*, const Unit*> adeptTargets;

    // TODO: calculate exact damage using upgrades info
    int adeptDamage = 22;

    // 2-pass algorithm:
    // first pass greedily matches adepts to workers. This could be improved by true
    for (auto&& lr : candidates) {
        int cnt = 0;
        for (auto adept : lr.second) {
            if (adeptTargets.count(adept) != 0) continue;
            ++cnt;
        }
        int totalHealth = lr.first->getHitPoints() + lr.first->getShields();
        if (adeptDamage * cnt > totalHealth) {
            int need = (totalHealth + adeptDamage - 1) / adeptDamage;
            for (auto adept : lr.second) {
                if (adeptTargets.count(adept) != 0) continue;
                adeptTargets[adept] = lr.first;
                if (--need == 0) {
                    break;
                }
            }
        }
    }
    // second pass forces adepts to attack workers if they can. Otherwise it doesn't really matter at the moment
    for (auto&& lr : candidates) {
        for (auto adept : lr.second) {
            if (adeptTargets.count(adept) != 0) continue;
            adeptTargets[adept] = lr.first;
        }
    }
    for (auto&& lr : adeptTargets) {
        lr.first->attackUnit(*lr.second);
    }
}

void AdeptHarassWorkers::onUnitAdded(const Unit *unit) {
    if (unit->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPT)) {
        adeptsAndShades.emplace_back(unit, nullptr);
        return;
    }
    if (!unit->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT)) {
        return;
    }
    std::pair<const Unit*, const Unit*>* adeptShadeRef = nullptr;
    float distToClosestAdept = -1;
    for (auto&& adept : adeptsAndShades) {
        if (adept.second != nullptr) {
            continue;
        }
        float dist = Util::Dist(*unit, *adept.first);
        if (adeptShadeRef == nullptr || distToClosestAdept < dist) {
            adeptShadeRef = &adept;
            distToClosestAdept = dist;
        }
    }
    BOT_ASSERT(adeptShadeRef != nullptr, "No adept found in squad");
    if (adeptShadeRef != nullptr) {
        adeptShadeRef->second = unit;
    }
}

void AdeptHarassWorkers::onUnitRemoved(const Unit *unit) {
    if (unit->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPT)) {
        for (auto it = adeptsAndShades.begin(); it != adeptsAndShades.end(); ++it) {
            if (it->first == unit) {
                it->first = nullptr;
                if (it->second == nullptr) {
                    adeptsAndShades.erase(it);
                }
                return;
            }
        }
    } else if (unit->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT)) {
        for (auto it = adeptsAndShades.begin(); it != adeptsAndShades.end(); ++it) {
            if (it->second == unit) {
                it->second = nullptr;
                if (it->first == nullptr) {
                    adeptsAndShades.erase(it);
                }
                return;
            }
        }
    }
}

AdeptHarassWorkers::AdeptHarassWorkers(CCBot &bot, Squad *squad, int currentTargetBase, int backupTargetBase) : Order(
        bot, squad), currentTargetBase(currentTargetBase), backupTargetBase(backupTargetBase) {
    shadeTarget = m_bot.Bases().getBaseLocation(currentTargetBase)->getPosition();
}
