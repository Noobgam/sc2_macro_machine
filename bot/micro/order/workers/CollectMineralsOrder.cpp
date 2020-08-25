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
    m_unassignedWorkers.insert(m_unassignedWorkers.end(), m_squad->units().begin(), m_squad->units().end());
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
        } else {
            // otherwise make sure that each probe is gathering the patch it was ordered to.
            // this will increase income and stabilize probes
            // it is better to validate it before reassigning probes from dead minerals so that probe doesn't get two orders in a row.
            for (auto worker : mineralWorkers.second) {
                auto&& workerOrders = worker->getUnitPtr()->orders;
                if (workerOrders.size() != 1) {
                    fixWorker(worker, *iter);
                } else {
                    auto&& order = workerOrders[0];
                    // sanity check. Shouldn't happen unless intervened externally
                    bool drills = worker->carriesResources() && order.ability_id == sc2::ABILITY_ID::HARVEST_GATHER;
                    bool minesOtherMineral =
                        order.ability_id == sc2::ABILITY_ID::HARVEST_GATHER
                            && order.target_unit_tag != (*iter)->getUnit()->getID();
                    if (drills || minesOtherMineral) {
                        fixWorker(worker, *iter);
                    }
                }
            }
        }
    }

    // reassign workers
    for (auto& mineralID : toDelete) {
        const auto& ptr = m_mineralToWorker.find(mineralID);
        BOT_ASSERT(ptr != m_mineralToWorker.end(), "Mineral was not found");
        m_unassignedWorkers.insert(m_unassignedWorkers.end(), ptr->second.begin(), ptr->second.end());
    }
    assignWorkers();
}

void CollectMineralsOrder::onUnitAdded(const Unit *unit) {
    BOT_ASSERT(unit->getType().isWorker(), "Can harvest minerals only with workers.");
    m_unassignedWorkers.emplace_back(unit);
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

void CollectMineralsOrder::assignWorkers() {
    auto& minerals = m_base->getBaseLocation()->getMinerals();
    for (auto workerIt = m_unassignedWorkers.begin(); workerIt != m_unassignedWorkers.end();) {
        auto bestIt = std::min_element(m_mineralToWorker.begin(), m_mineralToWorker.end(),
            [](const auto& mw1, const auto& mw2) {
                return mw1.second.size() < mw2.second.size();
            }
        );
        bestIt->second.emplace_back(*workerIt);
        auto id = bestIt->first;
        const auto& mineralIt = std::find_if(minerals.begin(), minerals.end(),
            [id](const auto& m) {
                return m->getID() == id;
            }
        );
        if (mineralIt != minerals.end()) {
            LOG_DEBUG << "Worker " << (*workerIt)->getID() << " was assigned on mineral " << (*mineralIt)->getID() << BOT_ENDL;
            (*workerIt)->gatherMineral(*(*mineralIt)->getUnit());
            workerIt = m_unassignedWorkers.erase(workerIt);
        } else {
            onEnd();
            break;
        }
    }
}

void CollectMineralsOrder::fixWorker(const Unit *worker, const Resource* mineral) const {
    LOG_DEBUG << "Fixing worker " << worker->getID() << BOT_ENDL;
    if (worker->carriesResources()) {
        worker->returnCargo();
    } else {
        worker->gatherMineral(*(mineral->getUnit()));
    }
}
