#include <util/LogInfo.h>
#include "CollectMineralsOrder.h"
#include "../../../general/bases/BaseLocation.h"

CollectMineralsOrder::CollectMineralsOrder(CCBot &bot, Squad *squad, const Base *base):
    Order(bot, squad),
    m_base(base)
{
    for (auto& mineral : base->getBaseLocation()->getMinerals()) {
        m_mineralToWorker.insert({mineral->getID(), {}});
    }
}

void CollectMineralsOrder::onStart() {
#ifdef _DEBUG
    for (auto& unit : m_squad->units()) {
        BOT_ASSERT(unit->getType().isWorker(), "Can harvest minerals only with workers.");
    }
#endif
    assignWorkers(m_squad->units());
}

void CollectMineralsOrder::onStep() {
    auto minerals = m_base->getBaseLocation()->getMinerals();
    // clear empty patches
    std::vector<CCUnitID> toDelete;
    for (auto& mineralWorkers : m_mineralToWorker) {
        CCUnitID mineralID = mineralWorkers.first;
        const auto& iter = std::find_if(minerals.begin(), minerals.end(), [mineralID](auto& mineral) {
            return mineral->getID() == mineralID;
        });
        if (iter == minerals.end()) {
            toDelete.push_back(mineralID);
        }
    }
    // reassign workers
    std::set<const Unit*> unassignedWorkers;
    for (auto& mineralID : toDelete) {
        const auto& ptr = m_mineralToWorker.find(mineralID);
        BOT_ASSERT(ptr != m_mineralToWorker.end(), "Mineral was not found");
        unassignedWorkers.insert(ptr->second.begin(), ptr->second.end());
    }
    assignWorkers(unassignedWorkers);
}

void CollectMineralsOrder::onUnitAdded(const Unit *unit) {
    BOT_ASSERT(unit->getType().isWorker(), "Can harvest minerals only with workers.");
    assignWorkers({unit});
}

void CollectMineralsOrder::onUnitRemoved(const Unit *unit) {
    for (auto& mineralWorker : m_mineralToWorker) {
        auto& workers = mineralWorker.second;
        const auto& it = std::find(workers.begin(), workers.end(), unit);
        if (it != workers.end()) {
            workers.erase(it);
            break;
        }
    }
}

void CollectMineralsOrder::assignWorkers(const std::set<const Unit *>& workers) {
    if (!workers.empty()) {
        auto &&logLine = LOG_DEBUG << "About to assign workers to minerals: ";
        for (auto &worker : workers) {
            if (!worker->isValid()) {
                logLine << "null ";
            } else {
                logLine << worker->getID() << " ";
            }
        }
        logLine << endl;
    }
    auto& minerals = m_base->getBaseLocation()->getMinerals();
    for (auto& worker : workers) {
        auto bestIt = std::min_element(m_mineralToWorker.begin(), m_mineralToWorker.end(),
            [](const auto& mw1, const auto& mw2) {
                return mw1.second.size() < mw2.second.size();
            }
        );
        bestIt->second.emplace_back(worker);
        auto id = bestIt->first;
        const auto& mineralIt = std::find_if(minerals.begin(), minerals.end(),
            [id](const auto& m) {
                return m->getID() == id;
            }
        );
        if (mineralIt != minerals.end()) {
            worker->rightClick(*(*mineralIt)->getUnit());
        } else {
            onEnd();
        }
    }
}
