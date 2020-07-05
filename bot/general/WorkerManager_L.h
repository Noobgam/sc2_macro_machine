#pragma once

#include "WorkerData.h"

class Building;
class CCBot;

class WorkerManager_L
{
    CCBot & m_bot;

    mutable WorkerData  m_workerData;

    void setMineralWorker(const Unit & unit);
    
    void handleIdleWorkers();
    void handleGasWorkers();
    void handleRepairWorkers();

public:

    WorkerManager_L(CCBot & bot);

    void onStart();
    void onFrame();

    void finishedWithWorker(const Unit & unit);
    void drawResourceDebugInfo();
    void drawWorkerInformation();
    void setScoutWorker(Unit worker);
    void setCombatWorker(Unit worker);
    void setBuildingWorker(Unit worker, Building & b);

    int  getNumMineralWorkers();
    int  getNumGasWorkers();
    int  getTotalWorkerCount();
    bool isWorkerScout(Unit worker) const;
    bool isFree(Unit worker) const;
    bool isBuilder(Unit worker) const;

    std::optional<Unit> getBuilder(Building & b,bool setJobAsBuilder = true) const;
    std::optional<Unit> getClosestDepot(Unit worker) const;
    std::optional<Unit> getGasWorker(Unit refinery) const;
    std::optional<Unit> getClosestMineralWorkerTo(const CCPosition & pos) const;
};

