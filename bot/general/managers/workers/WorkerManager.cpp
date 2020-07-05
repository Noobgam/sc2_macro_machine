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
    // process completed tasks
    std::vector<Squad*> completedSquads;
    for (auto& squad: m_additionalSquads) {
        if (squad->getOrder()->isCompleted()) {
            completedSquads.emplace_back(squad);
        }
    }
    for (auto& squad: completedSquads) {
        squadManager.deformSquad(squad);
    }
}

void WorkerManager::build(Unit *unit, UnitType type, CCPosition position) {
    Squad* squad = formSquad({unit});
    const auto& buildOrder = std::make_shared<EmptyOrder>(m_bot, squad);
    squad->setOrder(buildOrder);
}

std::optional<Squad*> WorkerManager::formSquad(int targetSquadSize) {
    auto freeWorkers = m_mainSquad->units();
    if (freeWorkers.size() < targetSquadSize) {
        return {};
    }
    // find workers for the task
    std::set<const Unit*> workers;
    for (auto& worker : freeWorkers) {
        if (workers.size() == targetSquadSize) {
            break;
        }
        workers.insert(worker);
    }
    // form squad
    return formSquad(workers);
}

Squad *WorkerManager::formSquad(const std::set<const Unit *> &workers) {
    auto& squadManager = m_bot.getManagers().getSquadManager();
    Squad* newSquad = squadManager.createNewSquad();
    squadManager.transferUnits(workers, newSquad);
    m_additionalSquads.emplace_back(newSquad);
    return newSquad;
}
