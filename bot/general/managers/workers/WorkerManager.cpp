#include <util/LogInfo.h>
#include "WorkerManager.h"
#include "../../CCBot.h"
#include "../../../micro/order/Orders.h"

WorkerManager::WorkerManager(CCBot &bot) : m_bot(bot) { }

void WorkerManager::onFrame() {
    assignFreeUnits();
    fixResourceLines(ResourceType::MINERAL);
    fixResourceLines(ResourceType::GEYSER);
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
    draw();
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
    const auto& completedBases = m_bot.getManagers().getBasesManager().getCompletedBases();
    BOT_ASSERT(!completedBases.empty(), "No active bases found");
    for (const auto& type : {ResourceType::GEYSER, ResourceType::MINERAL}) {
        for (auto& base : completedBases) {
            const auto & baseWorkers = base->getBaseWorkers();
            if (baseWorkers->getIdealResourceWorkers(type) > baseWorkers->getActiveResourceWorkers(type)) {
                baseWorkers->assignToResource(type, unit);
                return;
            }
        }
        for (auto& base : completedBases) {
            const auto & baseWorkers = base->getBaseWorkers();
            if (baseWorkers->getMaximumResourceWorkers(type) > baseWorkers->getActiveResourceWorkers(type)) {
                baseWorkers->assignToResource(type, unit);
                return;
            }
        }
    }
    completedBases[0]->getBaseWorkers()->assignToMineral(unit);
}

std::vector<const Unit*> WorkerManager::getFreeWorkers() {
    std::vector<const Unit*> freeWorkers;
    for (auto & base : m_bot.getManagers().getBasesManager().getBases()) {
        auto mineralUnits = base->getBaseWorkers()->getMineralSquad()->units();
        freeWorkers.insert(freeWorkers.end(), mineralUnits.begin(), mineralUnits.end());
    }
    return freeWorkers;
}

void WorkerManager::fixResourceLines(ResourceType type) {
    const auto& bases = m_bot.getManagers().getBasesManager().getCompletedBases();
    int availableWorkerPositions = 0;
    // try to make ideal saturation
    for (const auto& base : bases) {
        const auto& baseWorkers = base->getBaseWorkers();
        if (baseWorkers->getIdealResourceWorkers(type) > baseWorkers->getActiveResourceWorkers(type)) {
            availableWorkerPositions += baseWorkers->getIdealResourceWorkers(type) - baseWorkers->getActiveResourceWorkers(type);
        }
    }
    for (const auto& base : bases) {
        const auto& baseWorkers = base->getBaseWorkers();
        while (baseWorkers->getIdealResourceWorkers(type) < baseWorkers->getActiveResourceWorkers(type) && availableWorkerPositions > 0) {
            const Unit* worker = *(baseWorkers->getMineralSquad()->units().begin());
            assignUnit(worker);
            availableWorkerPositions--;
        }
    }
    // try to make any saturation
    availableWorkerPositions = 0;
    for (const auto& base : bases) {
        const auto& baseWorkers = base->getBaseWorkers();
        if (baseWorkers->getMaximumResourceWorkers(type) > baseWorkers->getActiveResourceWorkers(type)) {
            availableWorkerPositions += baseWorkers->getMaximumResourceWorkers(type) - baseWorkers->getActiveResourceWorkers(type);
        }
    }
    for (const auto& base : bases) {
        const auto& baseWorkers = base->getBaseWorkers();
        while (baseWorkers->getMaximumResourceWorkers(type) < baseWorkers->getActiveResourceWorkers(type) && availableWorkerPositions > 0) {
            const Unit* worker = *(baseWorkers->getMineralSquad()->units().begin());
            assignUnit(worker);
            availableWorkerPositions--;
        }
    }
}

void WorkerManager::draw() {
#ifdef _DEBUG
    const auto& bases = m_bot.getManagers().getBasesManager().getCompletedBases();
    std::stringstream ss;
    ss << "Bases: " << bases.size() << "\n";
    if (!bases.empty()) {
        ss << "Sizes: ";
        for (const auto & base: bases) {
            ss << base->getBaseWorkers()->getActiveMineralWorkers() << "/" << base->getBaseWorkers()->getIdealMineralWorkers() << " : ";
            ss << base->getBaseWorkers()->getActiveVespeneWorkers() << "/" << base->getBaseWorkers()->getMaximumVespeneWorkers() << ", ";
        }
        ss << "\n";
    }
    m_bot.Map().drawTextScreen(0.6f, 0.01f, ss.str(), CCColor(255, 255, 0));
#endif
}

