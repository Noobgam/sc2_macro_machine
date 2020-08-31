#include <util/LogInfo.h>
#include "BuildingTask.h"

#include "general/CCBot.h"

BuildingTask::BuildingTask(CCBot& bot, BuildingTaskID id, UnitType type, const Unit *builder, CCPosition position) :
        m_bot(bot),
        m_id(id),
        m_type(type),
        m_worker(builder),
        m_position(position),
        m_status(BuildingStatus::NEW),
        m_building() {
    m_bot.getManagers().getEconomyManager().reserveResource(ResourceType::MINERAL, m_bot.Data(type).mineralCost);
    m_bot.getManagers().getEconomyManager().reserveResource(ResourceType::VESPENE, m_bot.Data(type).gasCost);
}

BuildingTaskID BuildingTask::getId() const {
    return m_id;
}

UnitType BuildingTask::getType() const {
    return m_type;
}

std::optional<const Unit *> BuildingTask::getWorker() const {
    return m_worker;
}

CCPosition BuildingTask::getPosition() const {
    return m_position;
}

std::optional<const Unit *> BuildingTask::getBuilding() const {
    return m_building;
}

BuildingStatus BuildingTask::getStatus() const {
    return m_status;
}

void BuildingTask::scheduled() {
    LOG_DEBUG << "[BUILDING_MANAGER] Task " << m_id << " was scheduled." << BOT_ENDL;
    m_status = BuildingStatus::SCHEDULED;
}

void BuildingTask::ordered() {
    LOG_DEBUG << "[BUILDING_MANAGER] Task " << m_id << " was ordered." << BOT_ENDL;
    m_status = BuildingStatus::ORDERED;
    m_bot.getManagers().getEconomyManager().freeResource(ResourceType::MINERAL, m_bot.Data(m_type).mineralCost);
    m_bot.getManagers().getEconomyManager().freeResource(ResourceType::VESPENE, m_bot.Data(m_type).gasCost);
}

void BuildingTask::completed() {
    LOG_DEBUG << "[BUILDING_MANAGER] Task " << m_id << " is completed." << BOT_ENDL;
    m_status = BuildingStatus::COMPLETED;
    m_building = {};
}

void BuildingTask::buildingPlaced(const Unit *building) {
    LOG_DEBUG << "[BUILDING_MANAGER] Building for task " << m_id << " was placed. Id: " << building->getID() << BOT_ENDL;
    m_status = BuildingStatus::IN_PROGRESS;
    m_building = building;
    m_worker = {};
}

void BuildingTask::workerDied() {
    LOG_DEBUG << "[BUILDING_MANAGER] Worker died for task " << m_id << BOT_ENDL;
    if (m_status == BuildingStatus::SCHEDULED || m_status == BuildingStatus::NEW) {
        m_bot.getManagers().getEconomyManager().freeResource(ResourceType::MINERAL, m_bot.Data(m_type).mineralCost);
        m_bot.getManagers().getEconomyManager().freeResource(ResourceType::VESPENE, m_bot.Data(m_type).gasCost);
    }
    m_status = BuildingStatus::FAILED;
    m_worker = {};
}

void BuildingTask::buildingDied() {
    LOG_DEBUG << "[BUILDING_MANAGER] Building was destroyed while in progress for task " << m_id << BOT_ENDL;
    m_status = BuildingStatus::FAILED;
    m_building = {};
}

void BuildingTask::placementFailure() {
    LOG_DEBUG << "[BUILDING_MANAGER] Placement failed for task " << m_id << BOT_ENDL;
    m_status = BuildingStatus::FAILED;
}

std::ostream& operator <<(std::ostream& stream, BuildingStatus status) {
    switch (status) {
        case BuildingStatus::NEW:
            return stream << "NEW";
        case BuildingStatus::SCHEDULED:
            return stream << "SCHEDULED";
        case BuildingStatus::ORDERED:
            return stream << "ORDERED";
        case BuildingStatus::IN_PROGRESS:
            return stream << "IN_PROGRESS";
        case BuildingStatus::COMPLETED:
            return stream << "COMPLETED";
        default:
            return stream << "NONE";
    }
}

