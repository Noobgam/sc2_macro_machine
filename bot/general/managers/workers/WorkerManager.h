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

    void assignFreeUnits();
    void assignUnit(const Unit * unit);

    void fixResourceLines(ResourceType type);

    void draw();
public:
    explicit WorkerManager(CCBot & bot);

    void onFrame();

    std::vector<const Unit*> getFreeWorkers();

    // Forms new worker squad for the task. Do not forget to call onEnd for the order.
    std::optional<Squad*> formSquad(int targetSquadSize);
    Squad *formSquad(const std::set<const Unit *> &workers);
};
