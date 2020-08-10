#pragma once

#include "memory"

#include "../general/model/Common.h"
#include "BuildOrderItem.h"
#include "build_managers/BuildManager.h"
#include "../general/managers/buildings/BuildingManager.h"
#include "buildings/BuildingPlacer.h"

class CCBot;

class MacroManager
{
    CCBot &       m_bot;
    BuildingPlacer       m_buildingPlacer;

    std::vector<std::unique_ptr<BuildManager>> m_managers;
    std::string cachedProductionInformation;

    std::optional<BuildOrderItem>     getTopPriority();

    void                     produceIfPossible(BuildOrderItem item);
    void                     produce(const Unit* producer, BuildOrderItem item);

    bool    meetsReservedResources(const MetaType & type);
    bool    canMakeNow(const Unit* producer, const MetaType & type);
    int     getFreeMinerals();
    int     getFreeGas();

public:

    explicit MacroManager(CCBot & bot);

    void    onStart();
    void    onFrame();
    void    drawProductionInformation();
    BuildingPlacer& getBuildingPlacer();


    /**
        If you don't care about the position of trained / created unit you can use 1-arg func
    **/
    std::optional<const Unit*> getProducer(const MetaType& type);
};
