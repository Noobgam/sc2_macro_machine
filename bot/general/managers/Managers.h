#pragma once

#include <general/managers/resources/ResourceManager.h>
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
public:
    explicit Managers(CCBot & bot);

    void onStart();
    void onFrame();

    WorkerManager & getWorkerManager();
    SquadManager & getSquadManager();
    BuildingManager & getBuildingManager();
    ResourceManager & getResourceManager();
};
