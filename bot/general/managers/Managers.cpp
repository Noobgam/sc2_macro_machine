#include "Managers.h"
#include "general/CCBot.h"

Managers::Managers(CCBot & bot) :
    m_bot(bot),
    m_workerManager(bot),
    m_squadManager(bot),
    m_buildingManager(bot),
    m_resourceManager(bot)
{ }

WorkerManager &Managers::getWorkerManager() {
    return m_workerManager;
}

const WorkerManager &Managers::getWorkerManager() const {
    return m_workerManager;
}

SquadManager &Managers::getSquadManager() {
    return m_squadManager;
}

const SquadManager &Managers::getSquadManager() const {
    return m_squadManager;
}

BuildingManager &Managers::getBuildingManager() {
    return m_buildingManager;
}

const BuildingManager &Managers::getBuildingManager() const {
    return m_buildingManager;
}

ResourceManager &Managers::getResourceManager() {
    return m_resourceManager;
}

const ResourceManager &Managers::getResourceManager() const {
    return m_resourceManager;
}

void Managers::onStart() {
    m_bot.Bases().onStart();
    m_workerManager.onStart();
}

void Managers::onFrame() {
    m_workerManager.onFrame();
    m_squadManager.onFrame();
    m_buildingManager.onFrame();
}
