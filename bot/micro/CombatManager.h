#pragma once

#include "../general/model/Common.h"
#include "Squad_L.h"
#include "SquadData.h"
#include "squads/SquadManager.h"

class CCBot;

class CombatManager
{
    CCBot &         m_bot;
    SquadManager    m_squadManager;

    Squad* mainSquadID;

    void reformSquads();
public:

    explicit CombatManager(CCBot & bot);

    void onStart();
    void onFrame();
};

