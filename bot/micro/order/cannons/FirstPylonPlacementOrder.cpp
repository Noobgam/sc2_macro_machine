#include "FirstPylonPlacementOrder.h"

#include <general/CCBot.h>
#include <util/Util.h>

void cannons::FirstPylonPlacementOrder::onStep() {
    if (!placer.has_value()) {
        onEnd();
        return;
    }
    int needPylons = pylonPlacement.pylonPositions.size();
    int needCannons = 0;
    if (needPylons == 0) {
        ++needCannons;
    }
    int needMinerals = std::max(0, needPylons) * 100 + std::max(0, needCannons) * 150;
    if (needMinerals > allocatedMinerals) {
        int need = needMinerals - allocatedMinerals;
        int allocated = m_bot.getManagers().getEconomyManager().allocateWithCap(ResourceType::MINERAL, need);
        allocatedMinerals += allocated;
        needMinerals -= allocated;
    }

    if (needMinerals > allocatedMinerals) {
        if (unbuiltPylons.size() > 1) {
            placer.value()->move(*unbuiltPylons.begin());
        } else if (unbuiltPylons.size() == 1) {
            placer.value()->move(pylonPlacement.cannonPlacements[0]);
        }
        return;
    }
    // this is ignoring buildingManager since this is a very fragile module
    // and has to move the probe in specific directions
    auto&& orders = placer.value()->getUnitPtr()->orders;
    if (orders.empty()) {
        if (scheduledPylons.empty()) {
            if (!unbuiltPylons.empty()) {
                auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
                placer.value()->build(
                        pylonType,
                        CCPosition(unbuiltPylons.begin()->x + 1, unbuiltPylons.begin()->y + 1)
                );
                scheduledPylons.push_back(*unbuiltPylons.begin());
                unbuiltPylons.erase(unbuiltPylons.begin());
            } else {
                if (scheduledCannons.empty()) {
                    auto cannonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON, m_bot);
                    placer.value()->build(
                            cannonType,
                            CCPosition(
                                    pylonPlacement.cannonPlacements[0].x + 1,
                                    pylonPlacement.cannonPlacements[0].y + 1
                            )
                    );
                }
            }
        }
    } else {
        if (orders.size() == 1) {
            if (orders[0].ability_id == sc2::ABILITY_ID::BUILD_PYLON) {
                if (unbuiltPylons.size() > 1) {
                    placer.value()->queuedMove(
                            CCPosition(
                                    unbuiltPylons.begin()->x + 1,
                                    unbuiltPylons.begin()->y + 1)
                    );
                } else {
                    placer.value()->queuedMove(CCPosition(
                            pylonPlacement.cannonPlacements[0].x + 1,
                            pylonPlacement.cannonPlacements[0].y + 1
                    ));
                }
            }
        }
    }
}

cannons::FirstPylonPlacementOrder::FirstPylonPlacementOrder(
        CCBot &bot,
        Squad *squad,
        const PylonPlacement &pylonPlacement
)
    : Order(bot, squad)
    , pylonPlacement(pylonPlacement)
{
    unbuiltPylons = pylonPlacement.pylonPositions;
    for (auto x : squad->units()) {
        if (x->getAPIUnitType() == sc2::UNIT_TYPEID::PROTOSS_PROBE) {
            placer = x;
        }
    }
}

void cannons::FirstPylonPlacementOrder::processBuilding(const Unit *newBuilding) {
    if (newBuilding->getPlayer() == Players::Self) {
        sc2::UnitTypeID buildingType = newBuilding->getAPIUnitType();
        if (buildingType == sc2::UNIT_TYPEID::PROTOSS_PYLON) {
            auto it = std::find_if(scheduledPylons.begin(), scheduledPylons.end(), [newBuilding](
                    const CCTilePosition& pos) {
                return newBuilding->getTilePosition() == CCTilePosition(pos.x + 1, pos.y + 1);
            });
            if (it != scheduledPylons.end()) {
                scheduledPylons.erase(it);
            }
        } else if (buildingType == sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON) {
            auto it = std::find_if(scheduledCannons.begin(), scheduledCannons.end(), [newBuilding](
                    const CCTilePosition& pos) {
                return newBuilding->getTilePosition() == CCTilePosition(pos.x + 1, pos.y + 1);
            });
            if (it != scheduledCannons.end()) {
                scheduledCannons.erase(it);
                onEnd();
            }
        }
    }
}

void cannons::FirstPylonPlacementOrder::onUnitRemoved(const Unit *unit) {
    if (placer == unit) {
        placer = {};
    }
}
