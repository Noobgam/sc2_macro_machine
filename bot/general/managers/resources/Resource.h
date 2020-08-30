#pragma once

#include <general/model/Unit.h>

typedef long long ResourceID;

class CCBot;

enum ResourceType {
    MINERAL = 0,
    VESPENE  = 1
};

class Resource {
private:
    const CCBot & m_bot;
    const Unit * m_unit;
    ResourceID m_id;
    ResourceType m_type;

    size_t m_lastUpdate = 0;
public:
    Resource(const CCBot & bot, const Unit * unit, ResourceID id);
    void updateUnit(const Unit * unit);

    ResourceID getID() const;
    size_t getLastUpdate() const;
    const Unit * getUnit() const;
    ResourceType getResourceType() const;
    bool isVisible() const;

    CCPosition getPosition() const;

    int getResourceAmount() const;
};
