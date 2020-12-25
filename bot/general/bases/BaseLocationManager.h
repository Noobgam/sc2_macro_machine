#pragma once

#include "BaseLocation.h"

class CCBot;

class BaseLocationManager {
    CCBot & m_bot;

    bool needRecalculation;

    std::map<BaseLocationID, std::unique_ptr<BaseLocation>> m_baseLocationData;
    std::vector<BaseLocation *>                             m_baseLocationPtrs;
    std::vector<std::vector<BaseLocation *>>                m_tileBaseLocations;

    std::vector<std::vector<const Resource *>> findResourceClusters() const;
    BaseLocation * getBaseLocation(const CCPosition & pos) const;
    BaseLocation * getBaseLocation(const Resource* resource) const;

public:
    BaseLocationManager(CCBot & bot);

    void onStart();
    void onFrame();

    void prepareRecalculation();

    void resourceExpiredCallback(const Resource* resource);

    const std::vector<BaseLocation *> & getBaseLocations() const;
    const BaseLocation* getBaseLocation(BaseLocationID id) const;
    std::optional<BaseLocation*> findBaseLocation(CCPosition position) const;

    CCPosition getNextExpansion(int player) const;
    void drawBaseLocations();
};
