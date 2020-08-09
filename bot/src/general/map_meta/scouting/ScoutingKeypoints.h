#pragma once

#include <vector>
#include <general/model/Common.h>
#include <general/map_meta/StaticMapMeta.h>

struct ScoutingKeyPoints {

    int baseLocationPosition;

    // Visiting these positions will guarantee you will have vision over proxy
    std::vector<CCTilePosition> closeProxyKeyPoints;

    // Keep in mind that tsp ordering is not the same as "perfect scout"
    //  because closeProxyKeyPoints is oblivious to the fact that unit scouts everything on the path between key points
    std::vector<size_t> closeProxyTspOrdering;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar
            & baseLocationPosition
            & closeProxyKeyPoints
            & closeProxyTspOrdering;
    }

    static ScoutingKeyPoints getScoutingKeyPoints(const StaticMapMeta& mapMeta, int baseLocationId);
    static std::vector <CCTilePosition> orderTilesPerfectly(
            const StaticMapMeta& mapMeta,
            CCTilePosition start,
            std::vector<CCTilePosition> tilesToVisit
    );
};


