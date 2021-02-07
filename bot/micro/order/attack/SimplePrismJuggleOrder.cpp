#include <micro/util/MicroUtil.h>
#include <util/LogInfo.h>
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


    auto&& enemies = m_bot.UnitInfo().getUnits(Players::Enemy);
    auto targetO = MicroUtil::findUnitWithHighestThreat(
            prism,
            5.5f,
            enemies
    );
    float neededDir = 0;
    if (targetO.has_value()) {
        float dy = targetO.value()->getPosition().y - prism->getPosition().y;
        float dx = targetO.value()->getPosition().x - prism->getPosition().x;
        neededDir = atan2(dy, dx);
    }

    for (auto&& unit : m_squad->units()) {
        if (unit->isOfType(sc2::UNIT_TYPEID::PROTOSS_WARPPRISM)) {
            continue;
        }

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
            if (targetO.has_value()) {
                unit->attackUnit(*targetO.value());
            } else {
                auto newTargetO = MicroUtil::findUnitWithHighestThreat(unit, enemies);
                if (newTargetO.has_value()) {
                    unit->attackUnit(*newTargetO.value());
                } else {
                    unit->rightClick(*prism);
                }
            }
        }
        // not the other way for poc around because prism cant lift two units the same frame probably.
        unit->rightClick(*prism);
    }
    bool readyToUnload = false;
    for (auto&& unit : prism->getUnitPtr()->passengers) {
        if (curFrame > weaponCdFrame[unit.tag]) {
            readyToUnload = true;
            break;
        }
    }
    bool prismHasOrder = false;
    if (readyToUnload) {
        bool directedCorrectly = true;
        if (targetO.has_value()) {
            float neededFacing = neededDir;
            // atan2 - [-M_PI:M_PI]
            // facing -       [0:2*M_PI]
            if (neededFacing < 0) {
                neededFacing += 2 * M_PI;
            }
//            if (abs(neededFacing - prism->getUnitPtr()->facing) < 0.15) {
//                prismHasOrder = true;
//                prism->move(*targetO.value());
//                directedCorrectly = false;
//            }
        }
        if (directedCorrectly) {
            for (auto&& unit : prism->getUnitPtr()->passengers) {
                if (curFrame > weaponCdFrame[unit.tag]) {
                    prism->unloadUnitById(unit.tag);
                    break;
                }
            }
        }
    }
    if (!prismHasOrder && targetO.has_value()) {
        float dy = targetO.value()->getPosition().y - prism->getPosition().y;
        float dx = targetO.value()->getPosition().x - prism->getPosition().x;
        float ll = sqrt(dy * dy + dx * dx);
        dy *= 6 / ll;
        dx *= 6 / ll;

        auto pos = CCPosition{
            prism->getPosition().x - dx,
            prism->getPosition().y - dy
        };
        prism->move(pos);
    } else {
        prism->move(targetBase->getPosition());
    }
}

void SimplePrismJuggleOrder::onEnd() {
    Order::onEnd();
}

SimplePrismJuggleOrder::SimplePrismJuggleOrder(
        CCBot &bot,
        Squad *squad,
        const BaseLocation* targetBase
) : Order(bot, squad)
  , targetBase(targetBase)
{}
