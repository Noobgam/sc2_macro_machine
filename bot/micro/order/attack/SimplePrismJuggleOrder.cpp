#include <micro/util/MicroUtil.h>
#include "SimplePrismJuggleOrder.h"

void SimplePrismJuggleOrder::onStep() {
    const Unit* prism = NULL;
    for (auto&& unit : m_squad->units()) {
        if (unit->isOfType(sc2::UNIT_TYPEID::PROTOSS_WARPPRISM)) {
            prism = unit;
            break;
        }
    }
    if (prism == NULL) {
        return;
    }
    // very small so vector is best.
    std::vector<CCUnitID> unitsInPrism;
    for (auto&& unit : prism->getUnitPtr()->passengers) {
        unitsInPrism.push_back(unit.tag);
    }
    int curFrame = m_bot.GetCurrentFrame();

    for (auto&& unit : m_squad->units()) {
        auto it = std::find_if(unitsInPrism.begin(), unitsInPrism.end(), [unit](CCUnitID unitId) {
            return unitId == unit->getID();
        });
        if (it != unitsInPrism.end()) {
            continue;
        }
        if (unit->getWeaponCooldown() > 0.1) {
            weaponCdFrame[unit->getID()] = (curFrame + unit->getWeaponCooldown() + 0.99);
            unit->rightClick(*prism);
        } else {
            auto&& enemies = m_bot.UnitInfo().getUnits(Players::Enemy);
            auto targetO = MicroUtil::findUnitWithHighestThreat(unit, enemies);
            if (!targetO.has_value()) {
                unit->rightClick(*prism);
            } else {
                unit->attackUnit(*targetO.value());
            }
        }
        if (unit->isOfType(sc2::UNIT_TYPEID::PROTOSS_WARPPRISM)) {
            continue;
        } else {
            // not the other way for poc around because prism cant lift two units the same frame probably.
            unit->rightClick(*prism);
        }
    }
    for (auto&& unit : prism->getUnitPtr()->passengers) {
        if (curFrame > weaponCdFrame[unit.tag]) {
            prism->unloadUnitById(unit.tag);
        }
    }
}

void SimplePrismJuggleOrder::onEnd() {
    Order::onEnd();
}

SimplePrismJuggleOrder::SimplePrismJuggleOrder(CCBot &bot, Squad *squad) : Order(bot, squad) {}
