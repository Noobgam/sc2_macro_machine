#pragma once

#include "model/Common.h"
#include <map>

class StaticMapMeta;

class CCBot;

class DistanceMap 
{
    int m_width;
    int m_height;
    CCTilePosition m_startTile;

    // 2D matrix storing distances from the start tile
    std::vector<std::vector<int>> m_dist;

    std::vector<CCTilePosition> m_sortedTiles;

    void computeDistanceMap(
        std::function<bool(int,int)> walkable,
        const CCTilePosition startTile,
        const StaticMapMeta& mapMeta
    );
    
public:
    
    DistanceMap();
    void computeDistanceMap(CCBot & m_bot, const CCTilePosition & startTile);
    void computeDistanceMap(const StaticMapMeta & m_bot, const CCTilePosition & startTile);
    std::vector<CCTilePosition> getPathTo(CCTilePosition pos) const;

    int getDistance(int tileX, int tileY) const;
    int getDistance(const CCTilePosition & pos) const;
    int getDistance(const CCPosition & pos) const;

    // given a position, get the position we should move to to minimize distance
    const std::vector<CCTilePosition> & getSortedTiles() const;
    const CCTilePosition & getStartTile() const;

    void draw(CCBot & bot) const;
};