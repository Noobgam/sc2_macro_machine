#pragma once

#include "micro/order/Order.h"

#include <cannons/PylonPlacement.h>

namespace cannons {

    class FirstPylonPlacementOrder : public Order {
    private:
        const PylonPlacement pylonPlacement;

    public:
        FirstPylonPlacementOrder(CCBot &bot, Squad *squad, const PylonPlacement &pylonPlacement);
        void processBuilding(const Unit* newBuilding);

    private:

        std::optional<const Unit*> placer;
        std::vector<CCTilePosition> unbuiltPylons;
        std::vector<CCTilePosition> scheduledPylons;
        std::vector<CCTilePosition> scheduledCannons;
        int allocatedMinerals = 0;

        void onStep() override;
        void onUnitRemoved(const Unit *unit) override;

    public:

    };
}


