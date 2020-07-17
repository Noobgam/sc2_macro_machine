#include "CombatManager.h"

#include "general/CCBot.h"
#include "order/Orders.h"

CombatManager::CombatManager(CCBot & bot) : m_bot(bot) { }

void CombatManager::onStart() {
    mainSquad = m_bot.getManagers().getSquadManager().createNewSquad();
}

void CombatManager::onFrame() {
    reformSquads();
    if (mainSquad->units().size() >= 14 && !inAttack) {
        auto & base = *m_bot.Bases().getOccupiedBaseLocations(Players::Enemy).begin();
        mainSquad->setOrder(std::make_shared<AttackOrder>(m_bot, mainSquad, base->getPosition()));
        inAttack = true;
    }
}

void CombatManager::reformSquads() {
    auto& squadManager = m_bot.getManagers().getSquadManager();
    std::set<const Unit*> toTransfer;
    for (auto unit : squadManager.getUnassignedSquad()->units()) {
        if (unit->getType().isRegularUnit() && !unit->getType().isWorker()) {
            toTransfer.insert(unit);
        }
    }
    squadManager.transferUnits(toTransfer, mainSquad);
}

