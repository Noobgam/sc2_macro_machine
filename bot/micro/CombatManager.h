#pragma once

#include <micro/modules/ChronoBoostModule.h>
#include "../general/model/Common.h"
#include "../general/managers/squads/SquadManager.h"

class CCBot;

class CombatManager {
    CCBot &         m_bot;
    ChronoBoostModule     m_boostModule;

    Squad* mainSquad = nullptr;

    void reformSquads();
public:
    explicit CombatManager(CCBot & bot);

    void onStart();
    void onFrame();
};

