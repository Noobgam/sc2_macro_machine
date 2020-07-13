#pragma once

#include <general/model/Common.h>
#include <general/model/Unit.h>
#include <general/managers/resources/Mineral.h>

class CCBot;

class ResourceManager {
private:
    CCBot & m_bot;
    std::vector<std::unique_ptr<Mineral>> m_minerals = {};
    std::vector<const Mineral*> m_mineralPtrs = {};

    MineralID m_currentMineralID = 0;
public:
    explicit ResourceManager(CCBot & bot);

    const std::vector<const Mineral*>& getMinerals() const;

    void newUnitCallback(const Unit* unit);
    void unitDisappearedCallback(const Unit* unit);
};
