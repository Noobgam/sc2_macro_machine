#pragma once

#include "workers/WorkerManager.h"
#include "squads/SquadManager.h"

class CCBot;

class Managers {
private:
    CCBot & m_bot;
    WorkerManager m_workerManager;
    SquadManager m_squadManager;
public:
    explicit Managers(CCBot & bot);

    void onStart();
    void onFrame();

    WorkerManager & getWorkerManager();
    SquadManager & getSquadManager();
};
