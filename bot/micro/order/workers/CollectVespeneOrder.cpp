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
    assignWorkers(m_squad->units());
}

void CollectVespeneOrder::onStep() {
    auto assimilators = m_base->getActiveAssimilators();
    addActiveAssimilators(assimilators);
    // clear exhausted assimilators
    std::vector<CCUnitID> toDelete;
    std::set<const Unit*> unassignedWorkers;
    for (auto& vespeneWorkers : m_assimilatorToWorker) {
        CCUnitID assimilatorID = vespeneWorkers.first;
        const auto& iter = std::find_if(assimilators.begin(), assimilators.end(), [assimilatorID](auto& assimilator) {
            return assimilator.first->getID() == assimilatorID;
        });
        if (iter == assimilators.end() || iter->second->getResourceAmount() == 0) {
            toDelete.push_back(assimilatorID);
        }
        auto& workers = vespeneWorkers.second;
        // do not try to make ideal saturation
        while (workers.size() > 3) {
            unassignedWorkers.insert(workers.front());
            workers.erase(workers.begin());
        }
    }
    // reassign workers
    for (auto& assimilatorID : toDelete) {
        const auto& ptr = m_assimilatorToWorker.find(assimilatorID);
        BOT_ASSERT(ptr != m_assimilatorToWorker.end(), "Assimilator was not found");
        unassignedWorkers.insert(ptr->second.begin(), ptr->second.end());
    }
    assignWorkers(unassignedWorkers);
}

void CollectVespeneOrder::onUnitAdded(const Unit *unit) {
    BOT_ASSERT(unit->getType().isWorker(), "Can harvest vespene only with workers.");
    assignWorkers({unit});
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

void CollectVespeneOrder::assignWorkers(const std::set<const Unit *>& workers) {
    const auto& assimilators = m_base->getActiveAssimilators();
    addActiveAssimilators(assimilators);
    for (auto& worker : workers) {
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
            bestIt->second.emplace_back(worker);
            worker->rightClick(*(*assimilatorIt).first);
        } else {
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

