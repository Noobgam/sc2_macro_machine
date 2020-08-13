#include "CombatManager.h"

#include "general/CCBot.h"
#include "order/Orders.h"

CombatManager::CombatManager(CCBot & bot) :
    m_bot(bot),
    m_boostModule(bot)
{}

void CombatManager::onStart() {
    mainSquad = m_bot.getManagers().getSquadManager().createNewSquad();
}

void CombatManager::onFrame() {
    reformSquads();
    if (mainSquad->units().size() >= 10) {
        Order* order = mainSquad->getOrder().get();
        if (order->isCompleted() || dynamic_cast<AttackWithKiting*>(order) == nullptr) {
            auto & base = *m_bot.Bases().getOccupiedBaseLocations(Players::Enemy).begin();
            mainSquad->setOrder(std::make_shared<AttackWithKiting>(m_bot, mainSquad, base->getPosition()));
        }
    } else if (mainSquad->units().size() > 2) {
        int startId = m_bot.Bases().getPlayerStartLocation(Players::Self)->getBaseId();
        auto& orderedBases = m_bot.Map().getStaticMapMeta().getOrderedBasesByStartLocationId().at(startId);
        for (auto it = orderedBases.rbegin(); it != orderedBases.rend(); ++it) {
            int baseId = *it;
            if (m_bot.getManagers().getBasesManager().isBaseOccupied(baseId)) {
                auto base = m_bot.Bases().getBaseLocation(baseId);
                mainSquad->setOrder(std::make_shared<GroupAroundOrder>(
                    m_bot,
                    mainSquad,
                    base->getDepotActualPosition(),
                    true
                ));
                break;
            }
        }
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
    squadManager.transferUnits(toTransfer, mainSquad);
}

