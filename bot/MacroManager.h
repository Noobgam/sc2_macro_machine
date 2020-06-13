#pragma once

#include "memory"

#include "Common.h"
#include "BuildOrder.h"
#include "BuildingManager.h"
#include "BuildOrderQueue.h"
#include "BuildManager.h"

class CCBot;

class MacroManager
{
    CCBot &       m_bot;

    std::vector<std::unique_ptr<BuildManager>> m_managers;
    BuildingManager m_buildingManager;
    BuildOrderQueue m_queue;

    BuildOrderItem     getTopPriority();

    void                     produceIfPossible(BuildOrderItem item);
    void                     produce(const Unit& producer, BuildOrderItem item);

    Unit    getClosestUnitToPosition(const std::vector<Unit> & units, CCPosition closestTo);
    bool    meetsReservedResources(const MetaType & type);
    bool    canMakeNow(const Unit & producer, const MetaType & type);
    int     getFreeMinerals();
    int     getFreeGas();

public:

    MacroManager(CCBot & bot);

    void    onStart();
    void    onFrame();
    void    drawProductionInformation();

    /**
        If you don't care about the position of trained / created unit you can use 1-arg func
    **/
    Unit getProducer(const MetaType& type);
    Unit getProducer(const MetaType & type, CCPosition closestTo);
};
