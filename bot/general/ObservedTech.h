#pragma once

#include <cstdint>
#include <vector>
#include <util/robin_hood.h>
#include "ObservedTech.h"
#include <sc2api/sc2_api.h>

class ObservedTech {

    robin_hood::unordered_flat_set<sc2::UPGRADE_ID> observedTech;
    robin_hood::unordered_flat_set<sc2::BUFF_ID> observedBuffes;
public:
    bool addObservedBuff(sc2::BUFF_ID buffId);
};


