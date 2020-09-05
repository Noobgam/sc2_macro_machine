#pragma once

#include <micro/modules/ChronoBoostModule.h>
#include <micro/modules/scout/ScoutModule.h>
#include <general/bases/BaseLocation.h>
#include "../general/model/Common.h"
#include "../general/managers/squads/SquadManager.h"

class CCBot;

class CombatManager {
    CCBot &         m_bot;
    ChronoBoostModule     m_boostModule;
    ScoutModule           m_scoutModule;

    Squad* mainSquad = nullptr;
    Squad* leftOverSquad = nullptr;

    void reformSquads();
    void orderToGroup(Squad* squad);
    const std::optional<const BaseLocation*> getAttackTarget();

    void addDefensiveUnitsToAttack();
public:
    explicit CombatManager(CCBot & bot);

    void onStart();
    void onFrame();
};

