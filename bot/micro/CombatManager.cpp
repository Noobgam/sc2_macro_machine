#include "CombatManager.h"

CombatManager::CombatManager(CCBot & bot) : m_bot(bot) {

}

void CombatManager::onStart() {
    mainSquadID = m_squadManager.createNewSquad();
}

void CombatManager::onFrame() {
    reformSquads();
}

void CombatManager::reformSquads() {
    if (!m_squadManager.getUnassignedSquad().isEmpty()) {
        m_squadManager.transferUnits(m_squadManager.unassignedSquadID, mainSquadID);
    }
}

