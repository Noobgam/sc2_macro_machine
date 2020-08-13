#include "CombatManager.h"

#include "general/CCBot.h"
#include "order/Orders.h"

CombatManager::CombatManager(CCBot & bot) :
    m_bot(bot),
    m_boostModule(bot),
    m_scoutModule(bot)
{}

void CombatManager::onStart() {
    mainSquad = m_bot.getManagers().getSquadManager().createNewSquad();
}

void CombatManager::onFrame() {
    reformSquads();
    if (mainSquad->units().size() >= 20 && !inAttack) {
        const auto& base = getAttackTarget();
        if (base.has_value()) {
            mainSquad->setOrder(std::make_shared<AttackWithKiting>(m_bot, mainSquad, base.value()->getPosition()));
            inAttack = true;
        }
    }
    if (inAttack && mainSquad->getOrder()->isCompleted()) {
        const auto& base = getAttackTarget();
        if (base.has_value()) {
            mainSquad->setOrder(std::make_shared<AttackWithKiting>(m_bot, mainSquad, base.value()->getPosition()));
        }
    }
    m_scoutModule.onFrame();
    m_boostModule.onFrame();
}

const std::optional<const BaseLocation*> CombatManager::getAttackTarget() {
    const auto & bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getOccupiedEnemyBaseLocations();
    if (!bases.empty()) {
        return *bases.begin();
    } else {
        const auto & allBases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getAllExpectedEnemyBaseLocations();
        if (!allBases.empty()) {
            return *allBases.begin();
        }
        return {};
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

