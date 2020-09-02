#pragma once

#include <general/model/Unit.h>

namespace Commands {
    bool retreat(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies);

    bool attackTargetWithHighestThreat(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies);

    bool kiteBack(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies);

    bool pushForward(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies);
    bool pushForward(const CCBot& bot, const Unit* unit, std::vector<const Unit*> enemies, CCPosition position);
}
