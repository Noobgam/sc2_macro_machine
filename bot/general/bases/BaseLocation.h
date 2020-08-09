#pragma once

#include "../model/Common.h"
#include "../DistanceMap.h"
#include "../model/Unit.h"
#include <map>
#include <vector>
#include <general/managers/resources/Resource.h>
#include <general/map_meta/StaticMapMeta.h>

typedef unsigned long long BaseLocationID;

class CCBot;

static const int NearBaseLocationTileDistance = 20;

class BaseLocation {
    CCBot &                     m_bot;
    DistanceMap                 m_distanceMap;

    CCPosition                  m_depotActualPosition;
    CCPosition                  m_centerOfResources;
    std::vector<const Resource*>    m_geysers;
    std::vector<const Resource*>    m_minerals;

    std::vector<CCPosition>     m_mineralPositions;
    std::vector<CCPosition>     m_geyserPositions;

    std::map<CCPlayer, bool>    m_isPlayerOccupying;
    std::map<CCPlayer, bool>    m_playerHasDepot;
    std::map<CCPlayer, bool>    m_isPlayerStartLocation;

    BaseLocationID              m_baseID;
    CCPositionType              m_left;
    CCPositionType              m_right;
    CCPositionType              m_top;
    CCPositionType              m_bottom;
    bool                        m_isStartLocation;

    void initialize(const std::vector<const Resource*> & resources);
    void finishInitialization();
public:
    BaseLocation(
            CCBot & bot,
            BaseLocationID baseID,
            BaseLocationProjection baseLocationProjection,
            const std::vector<const Resource*> & resources
    );
    BaseLocation(CCBot & bot, BaseLocationID baseID, const std::vector<const Resource*> & resources);
    int getGroundDistance(const CCPosition & pos) const;
    int getGroundDistance(const CCTilePosition & pos) const;

    bool isPlayerStartLocation(CCPlayer player) const;
    bool isMineralOnly() const;
    bool containsPosition(const CCPosition & pos, int distance = NearBaseLocationTileDistance) const;
    const CCPosition & getDepotActualPosition() const;
    const CCPosition & getPosition() const;
    const std::vector<const Resource*> & getGeysers() const;
    const std::vector<const Resource*> & getMinerals() const;
    bool isOccupiedByPlayer(CCPlayer player) const;
    bool hasPlayerDepot(CCPlayer player) const;
    bool isExplored() const;
    BaseLocationID  getBaseId() const;
    bool isInResourceBox(int x, int y) const;

    void setStartLocation(CCPlayer player);
    void setPlayerHasDepot(CCPlayer player, bool hasDepot);
    void setPlayerOccupying(CCPlayer player, bool occupying);

    void resourceExpiredCallback(const Resource* resource);

    void draw() const;
};
