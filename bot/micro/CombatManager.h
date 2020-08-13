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
    bool inAttack = false;

    void reformSquads();
    const std::optional<const BaseLocation*> getAttackTarget();
public:
    explicit CombatManager(CCBot & bot);

    void onStart();
    void onFrame();
};

