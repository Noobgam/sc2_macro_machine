#pragma once

#include <vector>
#include <general/model/Common.h>

class PylonPlacement {
    std::vector<CCPosition> pylonPositions;
    // You can cancel a pylon and instantly replace it with another one.
    // pylon placements with replacements are ideal because probe can almost always escape after building a cannon this way
    std::map<CCPosition, CCPosition> pylonReplacements;
};


