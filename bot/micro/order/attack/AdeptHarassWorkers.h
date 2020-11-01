#pragma once

#include "../Order.h"
#include <boost/container/flat_map.hpp>

using boost::container::flat_map;

// this class exists because I'm quite unsure
// whether having different unit types in harass squad is a good idea.
// multiple squads could have the same target
// but sync units of generic types is very tedious to code and benefits are very much nonexistent
class AdeptHarassWorkers : public Order {

    static constexpr int SHADE_COOLDOWN = (11 * 22.4 + 10);

    std::vector<std::pair<const Unit*, const Unit*>> adeptsAndShades;
    // https://github.com/Noobgam/sc2_macro_machine/issues/140
    flat_map<const Unit*, int> lastShadeCast;
    //adepts shade between two bases
    int currentTargetBase;
    int backupTargetBase;
    CCPosition shadeTarget;


    // processes shades
    // if returns true - all adepts already have an action scheduled, skip the following frame.
    bool processShades();
    void processAdepts();

public:

    AdeptHarassWorkers(CCBot &bot, Squad *squad, int currentTargetBase, int backupTargetBase);

    void onStep() override;

    void onUnitAdded(const Unit *unit) override;

    void onUnitRemoved(const Unit *unit) override;
};


