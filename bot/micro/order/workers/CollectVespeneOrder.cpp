#include "CollectVespeneOrder.h"

CollectVespeneOrder::CollectVespeneOrder(CCBot &bot, Squad *squad, const Base *base):
    Order(bot, squad),
    m_base(base)
{ }

void CollectVespeneOrder::onStart() {
#ifdef _DEBUG
    for (auto& unit : m_squad->units()) {
        BOT_ASSERT(unit->getType().isWorker(), "Can harvest vespene only with workers.");
    }
#endif
    m_unassignedWorkers.insert(m_unassignedWorkers.end(), m_squad->units().begin(), m_squad->units().end());
    assignWorkers();
}

void CollectVespeneOrder::onStep() {
    auto assimilators = m_base->getActiveAssimilators();
    addActiveAssimilators(assimilators);
    // clear exhausted assimilators
    std::vector<CCUnitID> toDelete;
    for (auto& vespeneWorkers : m_assimilatorToWorker) {
        CCUnitID assimilatorID = vespeneWorkers.first;
        const auto& iter = std::find_if(assimilators.begin(), assimilators.end(), [assimilatorID](auto& assimilator) {
            return assimilator.first->getID() == assimilatorID;
        });
        if (iter == assimilators.end() || iter->second->getResourceAmount() == 0) {
            toDelete.push_back(assimilatorID);
        }
    }
    for (auto& assimilatorID : toDelete) {
        const auto& ptr = m_assimilatorToWorker.find(assimilatorID);
        BOT_ASSERT(ptr != m_assimilatorToWorker.end(), "Assimilator was not found");
        m_unassignedWorkers.insert(m_unassignedWorkers.end(), ptr->second.begin(), ptr->second.end());
        m_assimilatorToWorker.erase(assimilatorID);
    }
    // collect available worker positions
    int workersPositions = 0;
    for (auto& vespeneWorkers : m_assimilatorToWorker) {
        if (vespeneWorkers.second.size() < 3) {
            workersPositions += 3 - vespeneWorkers.second.size();
        }
    }
    // collect excessive workers
    for (auto& vespeneWorkers : m_assimilatorToWorker) {
        auto& workers = vespeneWorkers.second;
        if (workers.size() > 3 && workersPositions > m_unassignedWorkers.size()) {
            m_unassignedWorkers.emplace_back(workers.front());
            workers.erase(workers.begin());
        }
    }
    assignWorkers();
}

void CollectVespeneOrder::onUnitAdded(const Unit *unit) {
    BOT_ASSERT(unit->getType().isWorker(), "Can harvest vespene only with workers.");
    m_unassignedWorkers.emplace_back(unit);
}

void CollectVespeneOrder::onUnitRemoved(const Unit *unit) {
    for (auto& mineralWorker : m_assimilatorToWorker) {
        auto& workers = mineralWorker.second;
        const auto& it = std::find(workers.begin(), workers.end(), unit);
        if (it != workers.end()) {
            workers.erase(it);
            break;
        }
    }
}

void CollectVespeneOrder::assignWorkers() {
    const auto& assimilators = m_base->getActiveAssimilators();
    for (auto workerIt = m_unassignedWorkers.begin(); workerIt < m_unassignedWorkers.end();) {
        auto bestIt = std::min_element(m_assimilatorToWorker.begin(), m_assimilatorToWorker.end(),
            [](const auto& w1, const auto& w2) {
                return w1.second.size() < w2.second.size();
            }
        );
        auto id = bestIt->first;
        const auto& assimilatorIt = std::find_if(assimilators.begin(), assimilators.end(),
            [id](const auto& a) {
                return a.first->getID() == id;
            }
        );
        if (assimilatorIt != assimilators.end()) {
            bestIt->second.emplace_back(*workerIt);
            (*workerIt)->rightClick(*(*assimilatorIt).first);
            m_unassignedWorkers.erase(workerIt);
        } else {
            break;
            // Do nothing?
        }
    }
}

void CollectVespeneOrder::addActiveAssimilators(const std::vector<std::pair<const Unit *, const Resource *>> &assimilators) {
    if (assimilators.size() > m_assimilatorToWorker.size()) {
        for (auto& assimilator : assimilators) {
            if (assimilator.second->getResourceAmount() != 0) {
                m_assimilatorToWorker.insert({assimilator.first->getID(), {}});
            }
        }
    }
}

