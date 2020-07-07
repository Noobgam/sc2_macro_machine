#pragma once

#include "../../model/Unit.h"
#include "../../model/Common.h"
#include "../../../micro/squads/Squad.h"
#include "../../../macro/buildings/BuildingTask.h"

class CCBot;

class WorkerManager {
    CCBot & m_bot;
    Squad* m_mainSquad = nullptr;
    std::vector<Squad*> m_additionalSquads = {};

    Squad *formSquad(const std::set<const Unit *> &workers);
public:
    explicit WorkerManager(CCBot & bot);

    void onStart();
    void onFrame();

    void build(const BuildingTask* buildingTask);
    std::optional<Squad*> formSquad(int targetSquadSize);
};
