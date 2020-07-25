#pragma once

#include <general/managers/resources/ResourceManager.h>
#include <general/managers/economy/EconomyManager.h>
#include <general/managers/buildings/WallManager.h>
#include "workers/WorkerManager.h"
#include "squads/SquadManager.h"
#include "buildings/BuildingManager.h"

class CCBot;

class Managers {
private:
    CCBot & m_bot;
    WorkerManager   m_workerManager;
    SquadManager    m_squadManager;
    BuildingManager m_buildingManager;
    ResourceManager m_resourceManager;
    EconomyManager  m_economyManager;
    WallManager     m_wallManager;
public:
    explicit Managers(CCBot & bot);

    void onStart();
    void onFrame();

    WorkerManager & getWorkerManager();
    SquadManager & getSquadManager();
    BuildingManager & getBuildingManager();
    ResourceManager & getResourceManager();
    EconomyManager & getEconomyManager();
    WallManager & getWallManager();

    const WorkerManager & getWorkerManager() const;
    const SquadManager & getSquadManager() const;
    const BuildingManager & getBuildingManager() const;
    const ResourceManager & getResourceManager() const;
    const EconomyManager & getEconomyManager() const;
    const WallManager & getWallManager() const;
};
