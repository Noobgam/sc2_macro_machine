#pragma once

#include <vector>
#include <general/model/Common.h>

struct PylonPlacement {
    std::vector<CCTilePosition> pylonPositions;
    // You can cancel a pylon and instantly replace it with another one.
    // pylon placements with replacements are ideal because probe can almost always escape after building a cannon this way
    std::map<CCPosition, CCPosition> pylonReplacements;
};


