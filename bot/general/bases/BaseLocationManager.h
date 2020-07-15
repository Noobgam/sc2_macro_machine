#pragma once

#include "BaseLocation.h"

class CCBot;

class BaseLocationManager {
    CCBot & m_bot;

    std::vector<BaseLocation>                       m_baseLocationData;
    std::vector<const BaseLocation *>               m_baseLocationPtrs;
    std::map<CCPlayer, const BaseLocation *>        m_playerStartingBaseLocations;
    std::map<int, std::set<const BaseLocation *>>   m_occupiedBaseLocations;
    std::vector<std::vector<BaseLocation *>>        m_tileBaseLocations;

    BaseLocation * getBaseLocation(const CCPosition & pos) const;

public:

    BaseLocationManager(CCBot & bot);

    void onStart();
    void onFrame();

    void resourceExpiredCallback(const Resource* resource);

    const std::vector<const BaseLocation *> & getBaseLocations() const;
    const std::set<const BaseLocation *> & getOccupiedBaseLocations(int player) const;

    CCPosition getNextExpansion(int player) const;
    void drawBaseLocations();
};
