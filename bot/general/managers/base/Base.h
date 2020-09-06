#pragma once

#include <general/managers/workers/BaseWorkers.h>
#include <general/managers/resources/Resource.h>

class CCBot;
class BaseLocation;

class Base {
private:
    CCBot & m_bot;
    const BaseLocation * m_baseLocation;
    std::unique_ptr<BaseWorkers> m_workers;
    Squad* m_defensiveSquad;
    const Unit* m_nexus;

    std::vector<std::pair<const Unit*, const Resource*>> m_assimilators;
public:
    Base(CCBot & bot, const BaseLocation * baseLocation, const Unit* nexus);

    const BaseLocation * getBaseLocation() const;
    BaseWorkers * getBaseWorkers() const;
    Squad * getDefensiveSquad() const;
    const Unit * getNexus() const;

    std::vector<std::pair<const Unit*, const Resource*>> getAssimilators() const;
    std::vector<std::pair<const Unit*, const Resource*>> getActiveAssimilators() const;

    void onNewAssimilator(const Unit* assimilator, const Resource* geyser);
    void onAssimilatorDestroyed(const Unit* assimilator);
    void onDestroyed();
};
