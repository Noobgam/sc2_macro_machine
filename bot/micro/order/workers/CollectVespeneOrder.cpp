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
    auto assimilators = m_base->getAssimilators();
    if (assimilators.size() > m_assimilatorToWorker.size()) {
        for (auto& assimilator : assimilators) {
            m_assimilatorToWorker.insert({assimilator.first->getID(), {}});
        }
    }
    // clear exchausted assimilators
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
    // reassign workers
    std::set<const Unit*> unassignedWorkers;
    for (auto& assimilatorID : toDelete) {
        const auto& ptr = m_assimilatorToWorker.find(assimilatorID);
        BOT_ASSERT(ptr != m_assimilatorToWorker.end(), "Assimilator was not found");
        unassignedWorkers.insert(ptr->second.begin(), ptr->second.end());
    }
    assignWorkers(unassignedWorkers);
    // Task is completed if no minerals left
    if (assimilators.empty()) {
        onEnd();
    }
}

void CollectVespeneOrder::onUnitAdded(const Unit *unit) {
    BOT_ASSERT(unit->getType().isWorker(), "Can harvest minerals only with workers.");
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
    const auto& assimilators = m_base->getAssimilators();
    if (assimilators.size() > m_assimilatorToWorker.size()) {
        for (auto& assimilator : assimilators) {
            m_assimilatorToWorker.insert({assimilator.first->getID(), {}});
        }
    }
    for (auto& worker : workers) {
        auto bestIt = std::min_element(m_assimilatorToWorker.begin(), m_assimilatorToWorker.end(),
            [](const auto& mw1, const auto& mw2) {
                return mw1.second.size() < mw2.second.size();
            }
        );
        bestIt->second.emplace_back(worker);
        auto id = bestIt->first;
        const auto& assimilatorIt = std::find_if(assimilators.begin(), assimilators.end(),
            [id](const auto& a) {
                return a.first->getID() == id;
            }
        );
        if (assimilatorIt != assimilators.end()) {
            worker->rightClick(*(*assimilatorIt).first);
        } else {
            onEnd();
        }
    }
}
