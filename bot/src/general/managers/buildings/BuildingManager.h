#pragma once

#include "../../../macro/buildings/BuildingTask.h"

class CCBot;

class BuildingManager {
private:
    CCBot & m_bot;
    BuildingTaskID currentBuildingTaskID = 0;

    std::map<BuildingTaskID, std::unique_ptr<BuildingTask>> m_tasks;
    std::vector<BuildingTask*>                              m_tasksPtr;
public:
    BuildingManager(CCBot & bot);

    void onFrame();

    BuildingTask* newTask(const UnitType& type, const Unit* unit, CCPosition position);
    std::vector<BuildingTask*> getTasks();

    void newUnitCallback(const Unit* unit);
    void unitDisappearedCallback(const Unit* unit);
};