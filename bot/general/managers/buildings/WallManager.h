#pragma once

#include <optional>
#include <general/model/UnitType.h>
#include <general/map_meta/wall/WallPlacement.h>

class CCBot;

class WallManager {
private:
    CCBot & m_bot;
    bool needWall;

    int id = 0; // id of last placed building, to pay less attention to units early on
    std::vector<const Unit*> wallPartsAlive;
    std::optional<WallPlacement> chosenPlacement;
    int findChosenBuildingId(const Unit* unit) const;
public:
    WallManager(CCBot& bot);
    void onStart();
    // gets a location that is necessary for the wall or a location that would empower the wall
    // e.g. second line of gateways
    std::optional<CCPosition> getBuildLocation(const UnitType& b);
    void newUnitCallback(const Unit *unit);
    void processRemoveUnit(const Unit* unit);
    void draw();
};