#include "WorkerManager.h"
#include "../../CCBot.h"
#include "../../../micro/order/Orders.h"

WorkerManager::WorkerManager(CCBot &bot) : m_bot(bot) { }

void WorkerManager::onStart() {
    m_mainSquad = m_bot.getManagers().getSquadManager().createNewSquad();
}

void WorkerManager::onFrame() {
    auto& squadManager = m_bot.getManagers().getSquadManager();
    std::set<const Unit*> toTransfer;
    for (auto unit : squadManager.getUnassignedSquad()->units()) {
        if (unit->getType().isWorker()) {
            toTransfer.insert(unit);
        }
    }
    squadManager.transferUnits(toTransfer, m_mainSquad);
    if (dynamic_cast<EmptyOrder*>(m_mainSquad->getOrder().get()) != nullptr) {
        const BaseLocation* base = *m_bot.Bases().getOccupiedBaseLocations(Players::Self).begin();
        m_mainSquad->setOrder(std::make_shared<CollectMineralsOrder>(m_bot, m_mainSquad, base));
    }
}

void WorkerManager::build(Unit *unit, UnitType type, CCPosition position) {
    auto& squadManager = m_bot.getManagers().getSquadManager();
    Squad* squad = squadManager.createNewSquad();
    squadManager.transferUnits({unit}, squad);
}
