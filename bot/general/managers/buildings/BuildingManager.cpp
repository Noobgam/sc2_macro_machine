#include "BuildingManager.h"
#include "../../../util/LogInfo.h"
#include <general/CCBot.h>

BuildingManager::BuildingManager(CCBot &bot) : m_bot(bot) { }

void BuildingManager::onFrame() {
    for (auto & task : m_tasksPtr) {
        if (task->getStatus() == BuildingStatus::IN_PROGRESS && task->getBuilding().value()->isCompleted()) {
            task->completed();
        }
    }
    for (auto it = m_tasksPtr.begin(); it < m_tasksPtr.end();) {
        auto & task = *it;
        if (task->getStatus() == BuildingStatus::COMPLETED || task->getStatus() == BuildingStatus::FAILED) {
            if (task->getWorker().has_value() && task->getWorker().value()) {
                Squad* squad = m_bot.getManagers().getSquadManager().getUnitSquad(task->getWorker().value());
                m_bot.getManagers().getSquadManager().deformSquad(squad);
            }
            m_tasks.erase(task->getId());
            it = m_tasksPtr.erase(it);
        } else {
            it++;
        }
    }
    draw();
}

BuildingTask *BuildingManager::newTask(const UnitType &type, const Unit *unit, CCPosition position) {
    BuildingTaskID id = currentBuildingTaskID++;
    auto iter = m_tasks.insert({id, std::make_unique<BuildingTask>(m_bot, id, type, unit, position)});
    m_bot.getManagers().getEconomyManager().reserveResource(ResourceType::MINERAL, m_bot.Data(type).mineralCost);
    m_bot.getManagers().getEconomyManager().reserveResource(ResourceType::VESPENE, m_bot.Data(type).gasCost);

    BuildingTask* ptr = iter.first->second.get();
    m_tasksPtr.emplace_back(ptr);
    LOG_DEBUG << "New task added: " << ptr->getId() << " " << ptr->getType().getName() << BOT_ENDL;
    return ptr;
}

std::vector<BuildingTask *> BuildingManager::getTasks() {
    return m_tasksPtr;
}

void BuildingManager::newUnitCallback(const Unit *unit) {
    if (unit->getPlayer() != Players::Self) {
        return;
    }
    if (unit->getUnitPtr()->display_type == sc2::Unit::DisplayType::Placeholder || !unit->getType().isBuilding()) {
        return;
    }
    for (auto task : m_tasksPtr) {
        if (
            task->getStatus() == BuildingStatus::ORDERED &&
            task->getType() == unit->getType() &&
            task->getPosition() == unit->getPosition()
        ) {
            LOG_DEBUG << "[BUILDING_MANAGER] Task in progress: " << task->getId() << " " << task->getType().getName() << BOT_ENDL;
            task->buildingPlaced(unit);
        }
    }
}

void BuildingManager::unitDisappearedCallback(const Unit *unit) {
    if (unit->getPlayer() != Players::Self) {
        return;
    }
    if (unit->getUnitPtr()->display_type == sc2::Unit::DisplayType::Placeholder) {
        return;
    }
    for (auto task : m_tasksPtr) {
        if (
                task->getStatus() == BuildingStatus::IN_PROGRESS &&
                task->getBuilding().value()->getID() == unit->getID()
        ) {
            LOG_DEBUG << "[BUILDING_MANAGER] Task canceled (building destroyed): " << task->getId() << " " << task->getType().getName() << BOT_ENDL;
            task->buildingDied();
        }
        if (
                (task->getStatus() == BuildingStatus::NEW || task->getStatus() == BuildingStatus::SCHEDULED || task->getStatus() == BuildingStatus::ORDERED) &&
                task->getWorker().value()->getID() == unit->getID()
        ) {
            LOG_DEBUG << "[BUILDING_MANAGER] Task canceled (worker died): " << task->getId() << " " << task->getType().getName() << BOT_ENDL;
            task->workerDied();
        }
    }
}

void BuildingManager::handleError(const SC2APIProtocol::ActionError& actionError) {
    auto tag = actionError.unit_tag();
    auto abilityId = actionError.ability_id();
    for (auto task : m_tasksPtr) {
        BuildingStatus status = task->getStatus();
        if (status != BuildingStatus::NEW && status != BuildingStatus::SCHEDULED && status != BuildingStatus::ORDERED) {
            continue;
        }
        auto workerTag = task->getWorker().value()->getID();
        auto buildAbility = m_bot.Data(task->getType()).buildAbility;
        if (workerTag == tag && abilityId == buildAbility) {
            task->placementFailure();
            m_bot.Commander().getMacroManager().getBuildingPlacer().freeTiles(task->getType(), task->getPosition());
        }
    }
}

void BuildingManager::draw() {
#ifdef _DEBUG
    std::stringstream ss;
    ss << "Building manager: " << "\n";
    for (const auto& task : m_tasksPtr) {
        ss << task->getId() << ": " << task->getStatus() << " " << task->getType().getName();
        if (task->getWorker().has_value()) {
            ss << " Worker: " << task->getWorker().value()->getID();
        }
        ss << "\n";
    }
    m_bot.Map().drawTextScreen(0.01f, 0.3f, ss.str(), CCColor(255, 255, 0));
#endif
}
