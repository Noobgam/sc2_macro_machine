#include "WorkerManager.h"
#include "../../CCBot.h"
#include "../../../micro/order/Orders.h"

WorkerManager::WorkerManager(CCBot &bot) : m_bot(bot) { }

void WorkerManager::onStart() { }

void WorkerManager::onFrame() {
    updateBasesSquads();
    assignFreeUnits();
    auto& squadManager = m_bot.getManagers().getSquadManager();
    // process completed tasks
    for (auto it = m_additionalSquads.begin(); it < m_additionalSquads.end();) {
        auto& squad = *it;
        if (squad->getOrder()->isCompleted()) {
            squadManager.deformSquad(squad);
            it = m_additionalSquads.erase(it);
        } else {
            it++;
        }
    }
}

void WorkerManager::build(UnitType type, CCPosition position) {
    auto freeWorkers = getFreeWorkers();
    BOT_ASSERT(!freeWorkers.empty(), "No worker for task.");
    const auto & worker = *freeWorkers.begin();
    const BuildingTask* task = m_bot.getManagers().getBuildingManager().newTask(type, worker, position);
    Squad* squad = formSquad({worker});
    const auto& buildOrder = std::make_shared<BuildingOrder>(m_bot, squad, task);
    squad->setOrder(buildOrder);
}

std::optional<Squad*> WorkerManager::formSquad(int targetSquadSize) {
    auto freeWorkers = getFreeWorkers();
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

void WorkerManager::updateBasesSquads() {
    for (auto & base : m_bot.Bases().getOccupiedBaseLocations(Players::Self)) {
        if (base->hasPlayerDepot(Players::Self)) {
            const auto& squad = m_bot.getManagers().getSquadManager().createNewSquad();
            squad->setOrder(std::make_shared<CollectMineralsOrder>(m_bot, squad, base));
            m_mineralSquads.insert({base->getID(), squad});
        }
    }
    std::vector<const Unit*> toTransfer;
    for (auto it = m_mineralSquads.begin(); it != m_mineralSquads.end();) {
        const auto & baseId = it->first;
        if (!m_bot.Bases().getBaseLocation(baseId)->hasPlayerDepot(Players::Self)) {
            const auto& squad = it->second;
            m_bot.getManagers().getSquadManager().deformSquad(squad);
            it = m_mineralSquads.erase(it);
        } else {
            it++;
        }
    }
}

void WorkerManager::assignFreeUnits() {
    std::set<const Unit*> toTransfer;
    for (auto unit : m_bot.getManagers().getSquadManager().getUnassignedSquad()->units()) {
        if (unit->getType().isWorker()) {
            toTransfer.insert(unit);
        }
    }
    for (const auto& unit : toTransfer) {
        assignUnit(unit);
    }
}

void WorkerManager::assignUnit(const Unit* unit) {
    const auto & squad = std::min_element(m_mineralSquads.begin(), m_mineralSquads.end(), [](auto & s1, auto & s2) {
       return s1.second->units().size() - s2.second->units().size();
    });
    m_bot.getManagers().getSquadManager().transferUnits({unit}, squad->second);
}

std::vector<const Unit*> WorkerManager::getFreeWorkers() {
    std::vector<const Unit*> freeWorkers;
    for (auto & squadPair : m_mineralSquads) {
        auto units = squadPair.second->units();
        freeWorkers.insert(freeWorkers.end(), units.begin(), units.end());
    }
    return freeWorkers;
}

