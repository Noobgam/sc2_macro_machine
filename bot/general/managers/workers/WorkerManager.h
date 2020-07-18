#pragma once

#include <general/bases/BaseLocation.h>
#include "../../model/Unit.h"
#include "../../model/Common.h"
#include "../../../micro/squads/Squad.h"
#include "../../../macro/buildings/BuildingTask.h"
#include "general/managers/workers/BaseWorkers.h"

class CCBot;

class WorkerManager {
    CCBot & m_bot;
    std::vector<std::unique_ptr<BaseWorkers>> m_baseWorkers = {};
    std::vector<BaseWorkers*> m_baseWorkersPtrs = {};
    std::vector<Squad*> m_additionalSquads = {};

    std::vector<const Unit*> getFreeWorkers();

    void assignFreeUnits();
    void assignUnit(const Unit * unit);
    void updateBasesSquads();
    void fixMineralLines();

    Squad *formSquad(const std::set<const Unit *> &workers);

    void draw();
public:
    explicit WorkerManager(CCBot & bot);

    void onStart();
    void onFrame();

    const std::vector<BaseWorkers*>&  getBaseWorkers() const;

    // Gives a new build task to a worker.
    void build(UnitType type, CCPosition position);

    // Forms new worker squad for the task. Do not forget to call onEnd for the order.
    std::optional<Squad*> formSquad(int targetSquadSize);
};
