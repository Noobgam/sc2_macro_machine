#include "CombatManager.h"

CombatManager::CombatManager(CCBot & bot) :
    m_bot(bot),
    mainSquad(m_squadManager.getUnassignedSquad()) { }

void CombatManager::onStart() {
    mainSquad = m_squadManager.createNewSquad();
}

void CombatManager::onFrame() {
    reformSquads();
}

void CombatManager::reformSquads() {
    if (!m_squadManager.getUnassignedSquad()->isEmpty()) {
        m_squadManager.transferUnits(m_squadManager.getUnassignedSquad(), mainSquad);
    }
}

