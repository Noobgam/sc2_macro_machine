#pragma once
#include <vector>
#include "PylonPlacement.h"

struct hash_pylon_placement {
    size_t operator()(const PylonPlacement &p) const {
        return p.hash_code;
    }
};

struct BaseAnalysis {
    std::unordered_set<PylonPlacement, hash_pylon_placement> pylonPlacements;
    int revision;
};