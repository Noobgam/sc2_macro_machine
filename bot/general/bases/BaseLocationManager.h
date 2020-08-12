#pragma once

#include "BaseLocation.h"

class CCBot;

class BaseLocationManager {
    CCBot & m_bot;

    std::map<BaseLocationID, std::unique_ptr<BaseLocation>> m_baseLocationData;
    std::vector<BaseLocation *>                             m_baseLocationPtrs;
    std::map<CCPlayer, const BaseLocation *>                m_playerStartingBaseLocations;
    std::map<int, std::set<const BaseLocation *>>           m_occupiedBaseLocations;
    std::vector<std::vector<BaseLocation *>>                m_tileBaseLocations;

    std::vector<std::vector<const Resource *>> findResourceClusters() const;
    BaseLocation * getBaseLocation(const CCPosition & pos) const;
    BaseLocation * getBaseLocation(const Resource* resource) const;

public:
    BaseLocationManager(CCBot & bot);

    void onStart();
    void onFrame();

    void resourceExpiredCallback(const Resource* resource);

    const BaseLocation* getBaseLocation(BaseLocationID id) const;
    const std::vector<BaseLocation *> & getBaseLocations() const;
    const std::set<const BaseLocation *> & getOccupiedBaseLocations(int player) const;
    const BaseLocation* getPlayerStartLocation(CCPlayer player) const;

    std::optional<BaseLocation*> findBaseLocation(CCPosition position) const;

    CCPosition getNextExpansion(int player) const;
    void drawBaseLocations();
};
