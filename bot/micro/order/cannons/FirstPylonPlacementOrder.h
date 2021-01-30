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
        std::vector<const Unit*> placedBuilings;
        std::vector<CCTilePosition> unbuiltPylons;
        std::vector<CCTilePosition> scheduledPylons;
        int allocatedMinerals = 0;

        void onStep() override;

    public:

    };
}


