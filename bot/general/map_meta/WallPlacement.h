#pragma once

#include <vector>
#include <boost/serialization/utility.hpp>

class CCBot;

enum class WallType {
    FullWall = 1,
    WallWithUnitGaps = 2,
    WallWithPylonGap = 3,
};

enum class BuildingType {
    PoweringPylon = 0,
    TwoByTwo      = 1,
    ThreeByThree  = 2
};

enum class GapType {
    OneByOne = 0,
    TwoByTwo = 1
};

struct WallPlacement {
    // because wall orientation and placement depends on start location id
    int startBaseLocationId;
    WallType wallType;

    // {{lx, ly}, type} - position of a left-bottom most tile, and building size
    std::vector<std::pair<std::pair<int,int>, BuildingType>> buildings;

    // {{lx, ly}, type} - position of a left-bottom most tile, and gap size
    std::vector<std::pair<std::pair<int,int>, GapType>>      gaps;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & wallType & buildings & gaps;
    }

    // needed for boost
    WallPlacement();
    WallPlacement(
            int startBaseLocationId,
            WallType wallType,
            std::vector<std::pair<std::pair<int,int>, BuildingType>> buildings,
            std::vector<std::pair<std::pair<int,int>, GapType>>      gaps
            );

    static std::vector<WallPlacement> getWallsForBaseLocation(
            const CCBot& bot,
            int baseLocationId,
            int startBaseLocationId,
            int enemyStartBaseLocationId
    );

    static WallPlacement fullWall(int startLocationId, std::vector<std::pair<std::pair<int,int>, BuildingType>>);
};


