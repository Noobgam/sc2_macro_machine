#include "BuildingTask.h"

BuildingTask::BuildingTask(BuildingTaskID id, UnitType type, Unit *builder, CCPosition position) :
        m_id(id),
        m_type(type),
        m_worker(builder),
        m_position(position),
        m_status(BuildingStatus::NEW),
        m_building() { }

BuildingTaskID BuildingTask::getId() const {
    return m_id;
}

UnitType BuildingTask::getType() const {
    return m_type;
}

std::optional<Unit *> BuildingTask::getWorker() const {
    return m_worker;
}

CCPosition BuildingTask::getPosition() const {
    return m_position;
}

std::optional<Unit *> BuildingTask::getBuilding() const {
    return m_building;
}

BuildingStatus BuildingTask::getStatus() const {
    return m_status;
}

void BuildingTask::completed() {
    m_status = BuildingStatus::COMPLETED;
    m_building = {};
}

void BuildingTask::buildingPlaced(Unit *building) {
    m_status = BuildingStatus::IN_PROGRESS;
    m_building = building;
    m_worker = {};
}

void BuildingTask::workerDied() {
    m_status = BuildingStatus::FAILED;
    m_worker = {};
}

void BuildingTask::buildingDied() {
    m_status = BuildingStatus::FAILED;
    m_building = {};
}

