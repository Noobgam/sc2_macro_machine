#pragma once

#include "../general/model/Common.h"
#include "../general/managers/squads/SquadManager.h"

class CCBot;

class CombatManager {
    CCBot &         m_bot;

    Squad* mainSquad = nullptr;
    bool inAttack = false;

    void reformSquads();
public:
    explicit CombatManager(CCBot & bot);

    void onStart();
    void onFrame();
};

