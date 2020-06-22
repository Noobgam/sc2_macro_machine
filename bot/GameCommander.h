#pragma once

#include "general/model/Common.h"
#include "util/Timer.hpp"
#include "macro/MacroManager.h"
#include "micro/CombatCommander.h"

class CCBot;

class GameCommander
{
    CCBot &                 m_bot;
    Timer                   m_timer;

    MacroManager         m_macroManager;

public:

    explicit GameCommander(CCBot & bot);

    void onStart();
    void onFrame();

    void drawDebugInterface();
    void drawGameInformation(int x, int y);

};
