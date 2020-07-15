#include "Managers.h"
#include "general/CCBot.h"

Managers::Managers(CCBot & bot) :
    m_bot(bot),
    m_workerManager(bot),
    m_squadManager(bot),
    m_buildingManager(bot),
    m_resourceManager(bot),
    m_economyManager(bot)
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

ResourceManager &Managers::getResourceManager() {
    return m_resourceManager;
}

EconomyManager &Managers::getEconomyManager() {
    return m_economyManager;
}

const EconomyManager &Managers::getEconomyManager() const {
    return m_economyManager;
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
