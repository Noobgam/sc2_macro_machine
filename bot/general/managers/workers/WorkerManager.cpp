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
    for (auto it = m_additionalSquads.begin(); it < m_additionalSquads.end();) {
        auto& squad = *it;
        if (squad->getOrder()->isCompleted()) {
            squadManager.deformSquad(squad);
            m_additionalSquads.erase(it);
        } else {
            it++;
        }
    }
}

void WorkerManager::build(UnitType type, CCPosition position) {
    BOT_ASSERT(!m_mainSquad->units().empty(), "No worker for task.");
    const auto & worker = *m_mainSquad->units().begin();
    const BuildingTask* task = m_bot.getManagers().getBuildingManager().newTask(type, worker, position);
    Squad* squad = formSquad({worker});
    const auto& buildOrder = std::make_shared<BuildingOrder>(m_bot, squad, task);
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
