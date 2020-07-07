#pragma once

#include "../../general/model/MetaType.h"
#include "../../general/model/Common.h"
#include "../../general/model/Unit.h"

typedef unsigned long long BuildingTaskID;

enum class BuildingStatus {
    NEW = 0,
    IN_PROGRESS = 1,
    FAILED = 2,
    COMPLETED = 3
};

class BuildingTask {
    BuildingTaskID m_id;
    UnitType m_type;
    std::optional<const Unit*> m_worker;
    CCTilePosition m_position;
    BuildingStatus m_status;
    std::optional<const Unit*> m_building;
public:
    BuildingTask(BuildingTaskID id, UnitType type, const Unit* builder, CCTilePosition position);

    BuildingTaskID getId() const;
    UnitType getType() const;
    std::optional<const Unit*> getWorker() const;
    CCTilePosition getPosition() const;
    std::optional<const Unit*> getBuilding() const;
    BuildingStatus getStatus() const;

    void completed();
    void buildingPlaced(const Unit* building);
    void workerDied();
    void buildingDied();
};
