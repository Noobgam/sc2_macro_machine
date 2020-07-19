#pragma once

#include <vector>
#include <boost/serialization/utility.hpp>
#include <general/model/Common.h>

#include <general/map_meta/StaticMapMeta.h>

static int VERIFIER_THREAD_COUNT = 1;

void setVerifierThreadCount(int x);

class CCBot;

enum class WallType {
    FullWall = 1,
    WallWithUnitGaps = 2,
    WallWithPylonGap = 4,
    WallWithUnitAndPylonGap = WallWithPylonGap | WallWithUnitGaps
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
    int startLocationId;
    int baseLocationId;
    WallType wallType;

    // {{lx, ly}, type} - position of a left-bottom most tile, and building size
    std::vector<std::pair<std::pair<int,int>, BuildingType>> buildings;

    // {{lx, ly}, type} - position of a left-bottom most tile, and gap size
    std::vector<std::pair<std::pair<int,int>, GapType>>      gaps;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & startLocationId & baseLocationId & wallType & buildings & gaps;
    }

    // needed for boost
    WallPlacement();
    WallPlacement(
            int startLocationId,
            int baseLocationId,
            WallType wallType,
            std::vector<std::pair<std::pair<int,int>, BuildingType>> buildings,
            std::vector<std::pair<std::pair<int,int>, GapType>>      gaps
            );

    static std::vector<CCTilePosition> getTileCandidates(
            const StaticMapMeta& bot,
            int baseLocationId,
            int enemyLocationID
    );
    static std::vector<WallPlacement> getWallsForBaseLocation(
            const StaticMapMeta& mapMeta,
            int baseLocationId,
            int startBaseLocationId,
            int enemyStartBaseLocationId
    );


    static WallPlacement fullWall(
            int startLocationId,
            int baseLocationId,
            std::vector<std::pair<std::pair<int,int>, BuildingType>>
    );
};


