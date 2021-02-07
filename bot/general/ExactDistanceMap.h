#pragma once

#include <map>
#include <general/map_meta/StaticMapMeta.h>
#include <general/model/Common.h>
#include <util/Util.h>

using std::pair;
using std::hash;

// heavyweight class, holds very precise distances
class ExactDistanceMap {
    const int m_width;
    const int m_height;
    const CCTilePosition m_startTile;

    // max computed distance. <= 0 means infinity
    const int limit;
    std::function<int(int,int)> walkable;


public:

    ExactDistanceMap(
            int width,
            int height,
            const CCTilePosition &startTile,
            int limit,
            std::function<int(int, int)> walkable
    );
    std::unordered_map<pair<int,int>, float, hash_pair> m_dist;

};