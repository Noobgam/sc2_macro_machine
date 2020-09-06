#pragma once

#include <general/model/Unit.h>

// every command returns true if any command was given to unit and false otherwise
namespace Commands {
    // run to the safe position (without attacking enemies)
    bool retreat(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies);
    // try to attack unit with highest threat in the attack ragne
    bool attackTargetWithHighestThreat(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies);
    // attack enemy then retreat to the safe position
    bool kiteBack(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies);
    // move to target position while attacking close enemies
    bool pushForward(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies, CCPosition position);
    // try to kill every unit from the list
    bool destroyUnits(const CCBot &bot, const Unit *unit, std::vector<const Unit *> enemies);
}
