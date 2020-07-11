#pragma once

#include "memory"

#include "../general/model/Common.h"
#include "BuildingManager.h"
#include "BuildOrderItem.h"
#include "build_managers/BuildManager.h"

class CCBot;

class MacroManager
{
    CCBot &       m_bot;

    std::vector<std::unique_ptr<BuildManager>> m_managers;
    BuildingManager m_buildingManager;

    std::optional<BuildOrderItem>     getTopPriority();

    void                     produceIfPossible(BuildOrderItem item);
    void                     produce(const Unit& producer, BuildOrderItem item);

    std::optional<Unit>    getClosestUnitToPosition(const std::vector<Unit> & units, CCPosition closestTo);
    bool    meetsReservedResources(const MetaType & type);
    bool    canMakeNow(const Unit & producer, const MetaType & type);
    int     getFreeMinerals();
    int     getFreeGas();

public:

    explicit MacroManager(CCBot & bot);

    void    onStart();
    void    onFrame();
    void    drawProductionInformation();

    /**
        If you don't care about the position of trained / created unit you can use 1-arg func
    **/
    std::optional<Unit> getProducer(const MetaType& type);
    std::optional<Unit> getProducer(const MetaType & type, CCPosition closestTo);
};
