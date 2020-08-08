#include <util/Util.h>
#include <general/ThreatAnalyzer.h>
#include "AttackWithKiting.h"

AttackWithKiting::AttackWithKiting(CCBot &bot, Squad *squad, CCPosition position)
    : Order(bot, squad)
    , m_target_position(position) { }

void AttackWithKiting::onStart() {
}

void AttackWithKiting::onStep() {
    for (auto unit : m_squad->units()) {
        handleOneUnit(unit);
    }
}

void AttackWithKiting::onUnitAdded(const Unit *unit) {
}

void AttackWithKiting::handleOneUnit(const Unit *unit) {
    auto&& enemies = m_bot.UnitInfo().getUnits(Players::Enemy);
    float range = unit->getType().getAttackRange() + .5f;
    auto it = endangered.find(unit->getID());
    bool weaponOnCooldown = unit->getWeaponCooldown() > 0.1;
    bool inDanger;
    if (it != endangered.end()) {
        if (unit->shieldPercentage() > 0.8) {
            endangered.erase(it);
            inDanger = false;
        } else {
            inDanger = true;
        }
    } else {

        inDanger = unit->hpPercentage() < 0.2 && unit->getShields() == 0;
        if (inDanger) {
            endangered.insert(unit->getID());
        }
    }
    if (inDanger) {
        std::vector<std::pair<float, const Unit*>> targets;
        for (auto enemy : enemies) {
            float dist = Util::Dist(*enemy, *unit);
            targets.emplace_back(dist, enemy);
        }
        std::sort(targets.begin(), targets.end());
        std::vector<const Unit*> closeTargets;
        for (auto x : targets) {
            if (x.first > range + 3) {
                break;
            }
            closeTargets.push_back(x.second);
        }
        // if on cooldown - run away from targets who are too close
        if (closeTargets.empty()) {
            // if nobody is close enough A-click target
            unit->attackMove(m_target_position);
            return;
        }
        auto center = Util::CalcCenter(closeTargets);
        auto vec_to_center = center - unit->getPosition();
        auto normalized_backward = Util::NormalizeVector(vec_to_center * -1);
        unit->move(unit->getPosition() + normalized_backward);
    } else {
        std::vector<std::pair<float, const Unit*>> targets;
        for (auto enemy : enemies) {
            float dist = Util::Dist(*enemy, *unit);
            targets.emplace_back(dist, enemy);
        }
        std::sort(targets.begin(), targets.end());
        std::vector<const Unit*> closeTargets;
        float maxPriority = -1;
        const Unit* maxPriorityTarget = NULL;
        for (auto x : targets) {
            if (x.first > range + 2) {
                break;
            }
            closeTargets.push_back(x.second);
            float threat = ThreatAnalyzer::getUnitTypeThreat(x.second->getType(), unit->getType());
            if (threat > maxPriority) {
                maxPriority = std::max(threat, maxPriority);
                maxPriorityTarget = x.second;
            } else if (threat == maxPriority) {
                if (maxPriorityTarget->hpPercentage() > x.second->hpPercentage()) {
                    maxPriorityTarget = x.second;
                }
            }
        }
        if (closeTargets.empty()) {
            unit->attackMove(m_target_position);
        } else {
            unit->attackUnit(*maxPriorityTarget);
        }
    }
}
