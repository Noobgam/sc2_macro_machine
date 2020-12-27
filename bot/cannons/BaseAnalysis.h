#pragma once
#include <vector>
#include "PylonPlacement.h"

struct BaseAnalysis {
    std::vector<PylonPlacement> pylonPlacements;
    int revision;
};