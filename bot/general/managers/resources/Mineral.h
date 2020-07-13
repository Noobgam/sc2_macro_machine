#pragma once

#include <general/model/Unit.h>

typedef long long MineralID;

class CCBot;

class Mineral {
private:
    mutable CCBot * m_bot;
    const Unit * m_unit;
    UnitType m_unitType;
    MineralID m_id;

    size_t m_lastUpdate = 0;
public:
    Mineral(CCBot & bot, const Unit * unit, MineralID id);
    void updateUnit(const Unit * unit);

    MineralID getID() const;
    size_t getLastUpdate() const;
    const Unit * getUnit() const;
    bool isVisible() const;

    CCPosition getPosition() const;

    int getMineralsAmount() const;
};
