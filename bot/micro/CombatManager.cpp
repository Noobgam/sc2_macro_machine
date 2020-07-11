#include "CombatManager.h"

#include "general/CCBot.h"
#include "order/Orders.h"

CombatManager::CombatManager(CCBot & bot) :
    m_bot(bot),
    m_squadManager(bot),
    mainSquad(m_squadManager.getUnassignedSquad()),
    inAttack(false) { }

SquadManager & CombatManager::getSquadManager() {
    return m_squadManager;
}

void CombatManager::onStart() {
    mainSquad = m_squadManager.createNewSquad();
}

void CombatManager::onFrame() {
    reformSquads();
    if (mainSquad->units().size() > 22 && !inAttack) {
        auto & base = *m_bot.Bases().getOccupiedBaseLocations(Players::Enemy).begin();
        mainSquad->setOrder(std::make_shared<AttackOrder>(m_bot, mainSquad, base->getPosition()));
        inAttack = true;
    }

    m_squadManager.onFrame();
}

void CombatManager::reformSquads() {
    if (!m_squadManager.getUnassignedSquad()->isEmpty()) {
        m_squadManager.transferUnits(m_squadManager.getUnassignedSquad(), mainSquad);
    }
}

