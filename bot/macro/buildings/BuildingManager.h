#include "BuildingTask.h"

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

    BuildingTask* newTask(UnitType& type, Unit* unit, CCPosition position);
    std::vector<BuildingTask*> getTasks();

    void newUnitCallback(Unit* unit);
    void unitDiedCallback(Unit* unit);
};