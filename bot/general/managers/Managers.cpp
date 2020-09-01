#include <util/Util.h>
#include "Managers.h"
#include "general/CCBot.h"

Managers::Managers(CCBot & bot) :
    m_bot(bot),
    m_workerManager(bot),
    m_squadManager(bot),
    m_buildingManager(bot),
    m_resourceManager(bot),
    m_economyManager(bot),
    m_basesManager(bot),
    m_enemyManager(bot),
    m_wallManager(bot)
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

BasesManager &Managers::getBasesManager() {
    return m_basesManager;
}

EnemyManager &Managers::getEnemyManager() {
    return m_enemyManager;
}

WallManager &Managers::getWallManager() {
    return m_wallManager;
}

const WorkerManager &Managers::getWorkerManager() const {
    return m_workerManager;
}

const SquadManager &Managers::getSquadManager() const {
    return m_squadManager;
}

const BuildingManager &Managers::getBuildingManager() const {
    return m_buildingManager;
}

const ResourceManager &Managers::getResourceManager() const {
    return m_resourceManager;
}

const EconomyManager &Managers::getEconomyManager() const {
    return m_economyManager;
}

const BasesManager &Managers::getBasesManager() const {
    return m_basesManager;
}

const EnemyManager &Managers::getEnemyManager() const {
    return m_enemyManager;
}

const WallManager &Managers::getWallManager() const {
    return m_wallManager;
}

void Managers::onStart() {
    VALIDATE_CALLED_ONCE();

    m_resourceManager.onStart();
    m_bot.Bases().onStart();
    m_basesManager.onStart();
    m_squadManager.onStart();
    m_enemyManager.onStart();
    m_wallManager.onStart();
}

void Managers::onFrame() {
    m_economyManager.onFrame();
    m_workerManager.onFrame();
    m_squadManager.onFrame();
    m_buildingManager.onFrame();
    m_enemyManager.onFrame();
    m_wallManager.draw();
}
