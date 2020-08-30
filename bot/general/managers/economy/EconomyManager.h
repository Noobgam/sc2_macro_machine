#pragma once
#include "general/managers/resources/Resource.h"

class CCBot;

class EconomyManager {
private:
    CCBot & m_bot;

    int m_reservedMinerals = 0;
    int m_reservedVespene = 0;
public:
    explicit EconomyManager(CCBot & bot);

    double getMineralIncome() const;
    double getVespeneIncome() const;

    // returns amount of workerks that may gather minerals on currently occupied bases
    int getMineralWorkersPositions() const;
    // returns amount of workerks that may gather vespene on currently occupied bases
    int getVespeneWorkersPositions() const;

    void reserveResource(ResourceType type, int amount);
    void freeResource(ResourceType type, int amount);

    float getAvailableResources(ResourceType type, float seconds = 0) const ;
};
