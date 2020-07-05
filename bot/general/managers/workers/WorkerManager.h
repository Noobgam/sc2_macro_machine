#pragma once

#include "../../model/Unit.h"
#include "../../model/Common.h"
#include "../../../micro/squads/Squad.h"

class CCBot;

class WorkerManager {
    CCBot & m_bot;
    Squad* m_mainSquad = nullptr;
    std::vector<Squad*> m_additionalSquads = {};
public:
    explicit WorkerManager(CCBot & bot);

    void onStart();
    void onFrame();

    void build(Unit* unit, UnitType type, CCPosition position);
};
