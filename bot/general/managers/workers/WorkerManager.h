#pragma once

#include <general/bases/BaseLocation.h>
#include "../../model/Unit.h"
#include "../../model/Common.h"
#include "../../../micro/squads/Squad.h"
#include "../../../macro/buildings/BuildingTask.h"

class CCBot;

class WorkerManager {
    CCBot & m_bot;
    std::map<BaseLocationID, Squad*> m_mineralSquads = {};
    std::vector<Squad*> m_additionalSquads = {};

    std::vector<const Unit*> getFreeWorkers();

    void assignFreeUnits();
    void assignUnit(const Unit * unit);
    void updateBasesSquads();

    Squad *formSquad(const std::set<const Unit *> &workers);
public:
    explicit WorkerManager(CCBot & bot);

    void onStart();
    void onFrame();

    // Gives a new build task to a worker.
    void build(UnitType type, CCPosition position);

    // Forms new worker squad for the task. Do not forget to call onEnd for the order.
    std::optional<Squad*> formSquad(int targetSquadSize);
};
