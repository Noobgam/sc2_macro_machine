#pragma once

#include "../../general/model/MetaType.h"
#include "../../general/model/Common.h"
#include "../../general/model/Unit.h"

typedef unsigned long long BuildingTaskID;

enum BuildingStatus {
    NEW = 0,
    IN_PROGRESS = 1,
    FAILED = 2,
    COMPLETED = 3
};

class BuildingTask {
    BuildingTaskID m_id;
    UnitType m_type;
    std::optional<Unit*> m_worker;
    CCPosition m_position;
    BuildingStatus m_status;
    std::optional<Unit*> m_building;
public:
    BuildingTask(BuildingTaskID id, UnitType type, Unit* builder, CCPosition position);

    BuildingTaskID getId() const;
    UnitType getType() const;
    std::optional<Unit*> getWorker() const;
    CCPosition getPosition() const;
    std::optional<Unit*> getBuilding() const;
    BuildingStatus getStatus() const;

    void completed();
    void buildingPlaced(Unit* building);
    void workerDied();
    void buildingDied();
};
