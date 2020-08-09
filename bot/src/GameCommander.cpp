#include "GameCommander.h"
#include "general/CCBot.h"

GameCommander::GameCommander(CCBot & bot):
    m_bot            (bot),
    m_macroManager   (bot),
    m_combatManager  (bot) { }

MacroManager &GameCommander::getMacroManager() {
    return m_macroManager;
}

CombatManager &GameCommander::getCombatManager() {
    return m_combatManager;
}

void GameCommander::onStart() {
    m_macroManager.onStart();
    m_combatManager.onStart();
}

void GameCommander::onFrame() {
    m_timer.start();
    m_macroManager.onFrame();
    m_combatManager.onFrame();
}


