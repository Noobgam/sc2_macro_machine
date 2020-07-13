#pragma once

#include "BaseLocation.h"

class CCBot;

class BaseLocationManager {
    CCBot & m_bot;

    std::map<BaseLocationID, std::unique_ptr<BaseLocation>> m_baseLocationData;
    std::vector<const BaseLocation *>                       m_baseLocationPtrs;
    std::map<CCPlayer, const BaseLocation *>                m_playerStartingBaseLocations;
    std::map<int, std::set<const BaseLocation *>>           m_occupiedBaseLocations;
    std::vector<std::vector<BaseLocation *>>                m_tileBaseLocations;

    std::vector<std::vector<const Unit *>> findResourceClusters() const;
    BaseLocation * getBaseLocation(const CCPosition & pos) const;

public:
    BaseLocationManager(CCBot & bot);

    void onStart();
    void onFrame();
    void drawBaseLocations();

    void onNewUnitCallback(const Unit* unit);
    void onUnitDiedCallback(const Unit* unit);

    const BaseLocation* getBaseLocation(BaseLocationID id) const;
    const std::vector<const BaseLocation *> & getBaseLocations() const;
    const std::set<const BaseLocation *> & getOccupiedBaseLocations(int player) const;


    CCPosition getNextExpansion(int player) const;
};
