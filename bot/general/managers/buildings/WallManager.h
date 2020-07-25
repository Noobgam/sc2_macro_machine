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
    std::optional<WallPlacement> chosenPlacement;
public:
    WallManager(CCBot& bot);
    void onStart();
    // gets a location that is necessary for the wall or a location that would empower the wall
    // e.g. second line of gateways
    std::optional<CCPosition> getBuildLocation(const UnitType& b);
    void draw();
};