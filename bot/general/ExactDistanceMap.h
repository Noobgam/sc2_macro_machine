#pragma once

#include <map>
#include <general/map_meta/StaticMapMeta.h>
#include <general/model/Common.h>

using std::pair;
using std::hash;

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first * 10000);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

// heavyweight class, holds very precise distances
class ExactDistanceMap {
    const int m_width;
    const int m_height;
    const CCTilePosition m_startTile;

    // max computed distance. <= 0 means infinity
    const int limit;


public:

    ExactDistanceMap(const StaticMapMeta & m_bot, const CCTilePosition & startTile, int limit);
    std::unordered_map<pair<int,int>, float, hash_pair> m_dist;

};