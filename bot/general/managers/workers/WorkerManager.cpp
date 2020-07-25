#include <util/LogInfo.h>
#include "WorkerManager.h"
#include "../../CCBot.h"
#include "../../../micro/order/Orders.h"

WorkerManager::WorkerManager(CCBot &bot) : m_bot(bot) { }

void WorkerManager::onStart() { }

void WorkerManager::onFrame() {
    updateBasesSquads();
    assignFreeUnits();
    fixMineralLines();
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

const std::vector<BaseWorkers *> & WorkerManager::getBaseWorkers() const {
    return m_baseWorkersPtrs;
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
    for (const auto & base : m_bot.Bases().getOccupiedBaseLocations(Players::Self)) {
        if (base->hasPlayerDepot(Players::Self)) {
            const auto& it = std::find_if(m_baseWorkersPtrs.begin(), m_baseWorkersPtrs.end(), [base](const auto& bw) {
                return bw->getBaseLocation()->getID() == base->getID();
            });
            if (it == m_baseWorkersPtrs.end()) {
                const auto& value = m_baseWorkers.emplace_back(std::make_unique<BaseWorkers>(m_bot, base));
                m_baseWorkersPtrs.emplace_back(value.get());
            }
        }
    }
    std::vector<const Unit*> toTransfer;
    for (auto it = m_baseWorkers.begin(); it != m_baseWorkers.end();) {
        if (!(*it)->getBaseLocation()->hasPlayerDepot(Players::Self)) {
            (*it)->deform();
            m_baseWorkersPtrs.erase(std::find(m_baseWorkersPtrs.begin(), m_baseWorkersPtrs.end(), it->get()));
            it = m_baseWorkers.erase(it);
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
    for (auto& baseWorkers : m_baseWorkersPtrs) {
        if (baseWorkers->getIdealMineralWorkersNumber() > baseWorkers->getMineralSquad()->units().size()) {
            baseWorkers->assignToMineral(unit);
            return;
        }
    }
    for (auto& baseWorkers : m_baseWorkersPtrs) {
        if (baseWorkers->getMaximumMineralWorkersNumber() > baseWorkers->getMineralSquad()->units().size()) {
            baseWorkers->assignToMineral(unit);
            return;
        }
    }
    if (!m_baseWorkersPtrs.empty()) {
        m_baseWorkersPtrs[0]->assignToMineral(unit);
    } else {
        LOG_DEBUG << "[SURRENDER_REQUEST] Out of bases."<< endl;
    }
}

std::vector<const Unit*> WorkerManager::getFreeWorkers() {
    std::vector<const Unit*> freeWorkers;
    for (auto & baseWorker : m_baseWorkers) {
        auto mineralUnits = baseWorker->getMineralSquad()->units();
        freeWorkers.insert(freeWorkers.end(), mineralUnits.begin(), mineralUnits.end());
    }
    return freeWorkers;
}

void WorkerManager::fixMineralLines() {
    int availableWorkerPositions = 0;
    for (const auto& baseWorkers : m_baseWorkersPtrs) {
        if (baseWorkers->getIdealMineralWorkersNumber() > baseWorkers->getActiveMineralWorkersNumber()) {
            availableWorkerPositions += baseWorkers->getIdealMineralWorkersNumber() - baseWorkers->getActiveMineralWorkersNumber();
        }
    }
    for (const auto& baseWorkers : m_baseWorkersPtrs) {
        while (baseWorkers->getIdealMineralWorkersNumber() < baseWorkers->getActiveMineralWorkersNumber() && availableWorkerPositions > 0) {
            const Unit* worker = *(baseWorkers->getMineralSquad()->units().begin());
            assignUnit(worker);
            availableWorkerPositions--;
        }
    }

    availableWorkerPositions = 0;
    for (const auto& baseWorkers : m_baseWorkersPtrs) {
        if (baseWorkers->getMaximumMineralWorkersNumber() > baseWorkers->getActiveMineralWorkersNumber()) {
            availableWorkerPositions += baseWorkers->getMaximumMineralWorkersNumber() - baseWorkers->getActiveMineralWorkersNumber();
        }
    }
    for (const auto& baseWorkers : m_baseWorkersPtrs) {
        while (baseWorkers->getMaximumMineralWorkersNumber() < baseWorkers->getActiveMineralWorkersNumber() && availableWorkerPositions > 0) {
            const Unit* worker = *(baseWorkers->getMineralSquad()->units().begin());
            assignUnit(worker);
            availableWorkerPositions--;
        }
    }
}

void WorkerManager::draw() {
#ifdef _DEBUG
    std::stringstream ss;
    ss << "Bases: " << m_baseWorkers.size() << "\n";
    if (!m_baseWorkers.empty()) {
        ss << "Sizes: ";
        for (const auto & bw: m_baseWorkers) {
            ss << bw->getMineralSquad()->units().size() << "/" << bw->getIdealMineralWorkersNumber() << ", ";
        }
        ss << "\n";
    }
    m_bot.Map().drawTextScreen(0.6f, 0.01f, ss.str(), CCColor(255, 255, 0));
#endif
}

