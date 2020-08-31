#pragma once

#include "../../general/model/MetaType.h"
#include "../../general/model/Common.h"
#include "../../general/model/Unit.h"

typedef unsigned long long BuildingTaskID;

enum class BuildingStatus {
    NEW = 0,
    SCHEDULED = 1,
    ORDERED = 2,
    IN_PROGRESS = 3,
    FAILED = 4,
    COMPLETED = 5
};

std::ostream& operator <<(std::ostream& stream, BuildingStatus status);

class BuildingTask {
    CCBot& m_bot;
    BuildingTaskID m_id;
    UnitType m_type;
    std::optional<const Unit*> m_worker;
    CCPosition m_position;
    BuildingStatus m_status;
    std::optional<const Unit*> m_building;
public:
    BuildingTask(CCBot& bot, BuildingTaskID id, UnitType type, const Unit* builder, CCPosition position);

    BuildingTaskID getId() const;
    UnitType getType() const;
    std::optional<const Unit*> getWorker() const;
    CCPosition getPosition() const;
    std::optional<const Unit*> getBuilding() const;
    BuildingStatus getStatus() const;

    void scheduled();
    void ordered();
    void completed();
    void buildingPlaced(const Unit* building);
    void workerDied();
    void buildingDied();
    void placementFailure();
};
