#pragma once

#include <general/model/Unit.h>

typedef long long MineralID;

class CCBot;

enum ResourceType {
    MINERAL = 0,
    GEYSER  = 1
};

class Resource {
private:
    mutable CCBot * m_bot;
    const Unit * m_unit;
    MineralID m_id;
    ResourceType m_type;

    size_t m_lastUpdate = 0;
public:
    Resource(CCBot & bot, const Unit * unit, MineralID id);
    void updateUnit(const Unit * unit);

    MineralID getID() const;
    size_t getLastUpdate() const;
    const Unit * getUnit() const;
    ResourceType getResourceType() const;
    bool isVisible() const;

    CCPosition getPosition() const;

    int getResourceAmount() const;
};
