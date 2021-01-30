#pragma once

#include <vector>
#include <general/model/Common.h>

struct PylonPlacement {
    std::vector<CCTilePosition> pylonPositions;
    // You can cancel a pylon and instantly replace it with another one.
    // pylon placements with replacements are ideal because probe can almost always escape after building a cannon this way
    std::map<CCPosition, CCPosition> pylonReplacements;
    // cannon placements for this pylon placement which will allow you to understand which direction the probe has to
    // be moved in to allow it to build at least one cannon after pylon placement.
    std::vector<CCTilePosition> cannonPlacements;

    PylonPlacement(
            std::vector<CCTilePosition> pylonPositions,
            const std::map<CCPosition, CCPosition> &pylonReplacements,
            std::vector<CCTilePosition> cannonPlacements
    );

    size_t hash_code;

    bool operator==(const PylonPlacement& rhs) const;
};


