#pragma once

#include "../../general/model/Common.h"
#include "../../general/model/Unit.h"
#include "../../general/model/UnitType.h"

namespace BuildingStatus
{
    enum { Unassigned = 0, Assigned = 1, UnderConstruction = 2, Size = 3 };
}

class Building
{
public:

    CCTilePosition  desiredPosition;
    CCTilePosition  finalPosition;
    CCTilePosition  position;
    UnitType        type;
    std::optional<Unit> buildingUnit;
    std::optional<Unit> builderUnit;
    size_t          status;
    int             lastOrderFrame;
    bool            buildCommandGiven;
    bool            underConstruction;

    Building();

    // constructor we use most often
    Building(UnitType t, CCTilePosition desired);

    // equals operator
    bool operator == (const Building & b) const;
};