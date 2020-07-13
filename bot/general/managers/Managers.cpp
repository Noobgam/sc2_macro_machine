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

SquadManager &Managers::getSquadManager() {
    return m_squadManager;
}

BuildingManager &Managers::getBuildingManager() {
    return m_buildingManager;
}

void Managers::onStart() {
    m_bot.Bases().onStart();
    m_workerManager.onStart();
}

ResourceManager &Managers::getResourceManager() {
    return m_resourceManager;
}

void Managers::onFrame() {
    m_workerManager.onFrame();
    m_squadManager.onFrame();
    m_buildingManager.onFrame();
}
