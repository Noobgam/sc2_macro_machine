#pragma once

#include <general/model/Common.h>
#include <general/model/Unit.h>
#include <general/managers/resources/Resource.h>

class CCBot;

class ResourceManager {
private:
    CCBot & m_bot;
    std::vector<std::unique_ptr<Resource>> m_minerals = {};
    std::vector<std::unique_ptr<Resource>> m_geysers = {};
    std::vector<const Resource*> m_mineralPtrs = {};
    std::vector<const Resource*> m_geysersPtrs = {};

    MineralID m_currentResourceID = 0;

    void onResourceExpired(const Resource* resource);
public:
    explicit ResourceManager(CCBot & bot);

    const std::vector<const Resource*>& getMinerals() const;
    const std::vector<const Resource*>& getGeysers() const;

    void newUnitCallback(const Unit* unit);
    void unitDisappearedCallback(const Unit* unit);
};
