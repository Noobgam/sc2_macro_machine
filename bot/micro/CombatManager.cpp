#include "CombatManager.h"

#include "general/CCBot.h"
#include "order/Orders.h"

CombatManager::CombatManager(CCBot & bot) :
    m_bot(bot),
    m_boostModule(bot)
{}

void CombatManager::onStart() {
    mainSquad = m_bot.getManagers().getSquadManager().createNewSquad();
    leftOverSquad = m_bot.getManagers().getSquadManager().createNewSquad();
}

void CombatManager::onFrame() {
    reformSquads();
    if (mainSquad->units().size() >= 10 && !inAttack) {
        auto & base = *m_bot.Bases().getOccupiedBaseLocations(Players::Enemy).begin();
        mainSquad->setOrder(std::make_shared<AttackWithKiting>(m_bot, mainSquad, base->getPosition()));
        inAttack = true;
    }
    m_boostModule.onFrame();
}

void CombatManager::reformSquads() {
    auto& squadManager = m_bot.getManagers().getSquadManager();
    std::set<const Unit*> toTransfer;
    for (auto unit : squadManager.getUnassignedSquad()->units()) {
        if (unit->getType().isRegularUnit() && !unit->getType().isWorker()) {
            toTransfer.insert(unit);
        }
    }
    if (!inAttack) {
        squadManager.transferUnits(toTransfer, mainSquad);
    } else {
        squadManager.transferUnits(toTransfer, leftOverSquad);
        if (leftOverSquad->units().size() >= 8) {
            squadManager.transferUnits(leftOverSquad, mainSquad);
        }
    }
}

