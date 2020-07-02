#pragma once

#include "../general/model/Common.h"
#include "squads/SquadManager.h"

class CCBot;

class CombatManager {
    CCBot &         m_bot;
    SquadManager    m_squadManager;

    Squad* mainSquad;

    bool inAttack;

    void reformSquads();
public:
    explicit CombatManager(CCBot & bot);

    SquadManager & getSquadManager();

    void onStart();
    void onFrame();
};

