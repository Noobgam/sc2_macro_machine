#pragma once

#include "general/model/Common.h"
#include "util/Timer.hpp"
#include "macro/MacroManager.h"
#include "micro/CombatManager.h"

class GameCommander
{
    CCBot &                 m_bot;
    Timer                   m_timer;

    MacroManager         m_macroManager;
    CombatManager        m_combatManager;

public:
    explicit GameCommander(CCBot & bot);

    MacroManager& getMacroManager();
    CombatManager& getCombatManager();

    void onStart();
    void onFrame();
};
