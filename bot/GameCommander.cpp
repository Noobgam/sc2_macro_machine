#include "GameCommander.h"
#include "general/CCBot.h"

GameCommander::GameCommander(CCBot & bot)
    : m_bot                 (bot)
    , m_macroManager   (bot)
    , m_combatManager   (bot)
{

}

void GameCommander::onStart()
{
    m_macroManager.onStart();
    m_combatManager.onStart();
}

void GameCommander::onFrame()
{
    m_timer.start();
    m_macroManager.onFrame();
    m_combatManager.onFrame();

    drawDebugInterface();
}

void GameCommander::drawDebugInterface()
{
    drawGameInformation(4, 1);
}

void GameCommander::drawGameInformation(int x, int y)
{
}


