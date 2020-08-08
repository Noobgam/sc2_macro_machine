#pragma once

#include "../../general/model/Common.h"

class CCBot;
class BaseLocation;

class BuildingPlacer
{
    CCBot & m_bot;

    std::vector< std::vector<bool> > m_reserveMap;

    // queries for various BuildingPlacer data
    bool buildable(const UnitType & b, float x, float y) const;
    bool isReserved(int x, int y) const;
    bool isInResourceBox(int x, int y) const;
    bool isInAnyResourceBox(int x, int y) const;
    bool tileOverlapsBaseLocation(int x, int y, UnitType type) const;

    std::optional<CCPosition> getRefineryPosition() const;

public:

    BuildingPlacer(CCBot & bot);

    void onStart();

    // determines whether we can build at a given location
    bool canBuildHere(float x, float y, const UnitType & b) const;

    // determines whether we can build at a given location
    bool canBuildHereWithoutCoveringNexus(float x, float y, const UnitType & b) const;

    // returns a build location near a building's desired location
    std::optional<CCPosition> getBuildLocation(const UnitType & b) const;

    void drawReservedTiles();
    void reserveTiles(int x, int y, int width, int height);
    void freeTiles(int x, int y, int width, int height);
};
