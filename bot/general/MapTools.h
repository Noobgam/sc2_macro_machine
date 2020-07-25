#pragma once

#include "model/Common.h"
#include "DistanceMap.h"
#include "model/UnitType.h"
#include <memory>
#include "general/map_meta/MapMeta.h"

class MapTools
{
    CCBot & m_bot;
    float   m_maxZ;
    int     m_frame;
    std::unique_ptr<StaticMapMeta> m_staticMapMeta;
    

    // a cache of already computed distance maps, which is mutable since it only acts as a cache
    mutable std::map<std::pair<int,int>, DistanceMap>   m_allMaps;

    std::vector<std::vector<int>>   m_lastSeen;         // the last time any of our units has seen this position on the map
    std::vector<std::vector<int>>   m_powerMap;         // boolean map whether specific halftile is powered by our pylons
    std::vector<std::vector<bool>>  m_unbuildableNeutral;        // unbuildable rocks and plates
    std::vector<std::vector<bool>>  m_unwalkableNeutral;         // unbuildable rocks

    int getSectorNumber(int x, int y) const;
        
    void printMap();

    float   terrainHeight(const CCPosition & point) const;

    bool    canBuild(int tileX, int tileY);
    bool    canWalk(int tileX, int tileY);
    void    updatePowerMap();
    void    changePowering(const CCPosition& pylonPos, float radius, int d);
    void    powerPylon(const CCPosition& pylonPos, float r);
    void    depowerPylon(const CCPosition& pylonPos, float r);

public:

    MapTools(CCBot & bot);

    bool    pylonPowers(const CCPosition& pylonPos, float radius, const CCPosition& candidate) const;

    void    onStart();
    void    onFrame();
    void    draw() const;

    int     width() const;
    int     height() const;
    float   terrainHeight(float x, float y) const;

    void    drawLine(CCPositionType x1, CCPositionType y1, CCPositionType x2, CCPositionType y2, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawLine(const CCPosition & p1, const CCPosition & p2, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawTile(int tileX, int tileY, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawHalfTile(float x, float y, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawBox(CCPositionType x1, CCPositionType y1, CCPositionType x2, CCPositionType y2, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawBox(const CCPosition & tl, const CCPosition & br, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawCircle(CCPositionType x1, CCPositionType x2, CCPositionType radius, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawCircle(const CCPosition & pos, CCPositionType radius, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawGroundCircle(const CCPosition & pos, CCPositionType radius, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawText(const CCPosition & pos, const std::string & str, const CCColor & color = CCColor(255, 255, 255)) const;
    void    drawTextScreen(float xPerc, float yPerc, const std::string & str, const CCColor & color = CCColor(255, 255, 255)) const;
    
    bool    isValidTile(int tileX, int tileY) const;
    bool    isValidTile(const CCTilePosition & tile) const;
    bool    isValidPosition(const CCPosition & pos) const;
    // tile or halftile
    bool    isPowered(float x, float y) const;
    bool    isExplored(int tileX, int tileY) const;
    bool    isExplored(const CCPosition & pos) const;
    bool    isExplored(const CCTilePosition & pos) const;
    bool    isVisible(int tileX, int tileY) const;
    bool    isVisible(const CCTilePosition& from, const CCTilePosition& to, float R) const;
    bool    canBuildTypeAtPosition(float tileX, float tileY, const UnitType & type) const;

    const   DistanceMap & getDistanceMap(const CCTilePosition & tile) const;
    const   DistanceMap & getDistanceMap(const CCPosition & tile) const;
    int     getGroundDistance(const CCPosition & src, const CCPosition & dest) const;
    bool    isConnected(int x1, int y1, int x2, int y2) const;
    bool    isConnected(const CCTilePosition & from, const CCTilePosition & to) const;
    bool    isConnected(const CCPosition & from, const CCPosition & to) const;
    bool    isWalkable(int tileX, int tileY) const;
    bool    isWalkable(const CCTilePosition & tile) const;
    
    bool    isBuildable(int tileX, int tileY) const;
    bool    isBuildable(const CCTilePosition & tile) const;

    // returns
    // @first  number of new spots that would be powered
    // @second number of spots that would be double powered
    std::pair<int, int> assumePylonBuilt(const CCPosition& pos, float radius) const;

    // returns a list of all tiles on the map, sorted by 4-direcitonal walk distance from the given position
    const std::vector<CCTilePosition> & getClosestTilesTo(const CCTilePosition & pos) const;


    void updateNeutralMap();
    const StaticMapMeta& getStaticMapMeta() const;
};

