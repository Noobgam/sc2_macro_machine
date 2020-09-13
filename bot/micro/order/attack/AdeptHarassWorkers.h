#pragma once

#include "../Order.h"

// this class exists because I'm quite unsure
// whether having different unit types in harass squad is a good idea.
// multiple squads could have the same target
// but sync units of generic types is very tedious to code and benefits are very much nonexistent
class AdeptHarassWorkers : public Order {
    std::vector<std::pair<const Unit*, const Unit*>> adeptsAndShades;
    // https://github.com/Noobgam/sc2_macro_machine/issues/140

public:
    void onStep() override;
};


