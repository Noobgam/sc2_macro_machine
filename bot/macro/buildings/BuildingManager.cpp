#include "BuildingManager.h"

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
            m_tasks.erase(task->getId());
            it = m_tasksPtr.erase(it);
        } else {
            it++;
        }
    }
}

BuildingTask *BuildingManager::newTask(UnitType &type, Unit *unit, CCPosition position) {
    BuildingTaskID id = currentBuildingTaskID++;
    auto iter = m_tasks.insert({id, std::make_unique<BuildingTask>(id, type, unit, position)});
    BuildingTask* ptr = iter.first->second.get();
    m_tasksPtr.emplace_back(ptr);
    return ptr;
}

std::vector<BuildingTask *> BuildingManager::getTasks() {
    return m_tasksPtr;
}

void BuildingManager::newUnitCallback(Unit *unit) {
    if (unit->getUnitPtr()->display_type == sc2::Unit::DisplayType::Placeholder || !unit->getType().isBuilding()) {
        return;
    }
    for (auto task : m_tasksPtr) {
        if (
            task->getStatus() == BuildingStatus::NEW &&
            task->getType() == unit->getType() &&
            task->getPosition() == unit->getPosition()
        ) {
            task->buildingPlaced(unit);
        }
    }
}

void BuildingManager::unitDiedCallback(Unit *unit) {
    for (auto task : m_tasksPtr) {
        if (
                task->getStatus() == BuildingStatus::IN_PROGRESS &&
                task->getBuilding().value()->getID() == unit->getID()
        ) {
            task->buildingDied();
        }
        if (
                task->getStatus() == BuildingStatus::NEW &&
                task->getWorker().value()->getID() == unit->getID()
        ) {
            task->workerDied();
        }
    }
}
