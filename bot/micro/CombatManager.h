#pragma once

#include <micro/modules/BoostModule.h>
#include "../general/model/Common.h"
#include "../general/managers/squads/SquadManager.h"

class CCBot;

class CombatManager {
    CCBot &         m_bot;
    BoostModule     m_boostModule;

    Squad* mainSquad = nullptr;
    bool inAttack = false;

    void reformSquads();
public:
    explicit CombatManager(CCBot & bot);

    void onStart();
    void onFrame();
};

