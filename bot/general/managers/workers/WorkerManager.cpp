#include <util/LogInfo.h>
#include <util/Util.h>
#include "WorkerManager.h"
#include "../../CCBot.h"
#include "../../../micro/order/Orders.h"

WorkerManager::WorkerManager(CCBot &bot) : m_bot(bot) { }

void WorkerManager::onFrame() {
    auto& squadManager = m_bot.getManagers().getSquadManager();
    fixResourceLines(ResourceType::MINERAL);
    fixResourceLines(ResourceType::VESPENE);
    assignFreeUnits();
    draw();
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
    for (const auto& type : {ResourceType::VESPENE, ResourceType::MINERAL}) {
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
    if (!completedBases.empty()) {
        completedBases[0]->getBaseWorkers()->assignToMineral(unit);
    } else {
        LOG_DEBUG << "[SURRENDER_REQUEST] Out of bases."<< BOT_ENDL;
    }
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
            const Unit* worker = *(baseWorkers->getResourceSquad(type)->units().begin());
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
            const Unit* worker = *(baseWorkers->getResourceSquad(type)->units().begin());
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
        ss << BOT_ENDL;
    }
    m_bot.Map().drawTextScreen(0.6f, 0.01f, ss.str(), CCColor(255, 255, 0));
#endif
}

