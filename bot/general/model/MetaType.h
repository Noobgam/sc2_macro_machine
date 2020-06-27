#pragma once

#include "Common.h"
#include "UnitType.h"

namespace MetaTypes
{
    enum { Unit, Upgrade, Buff, Tech, Ability, None };
}

class CCBot;
class MetaType
{
    const CCBot *         m_bot;
    size_t          m_type;
    std::string     m_name;
    CCRace          m_race;
    UnitType        m_unitType;
    CCUpgrade       m_upgrade;
    
public:

    MetaType();
    MetaType(const std::string & name, CCBot & bot);
    MetaType(const UnitType & unitType, CCBot & bot);
    MetaType(const CCUpgrade & upgradeType, CCBot & bot);

    bool    isUnit()        const;
    bool    isUpgrade()     const;
    bool    isTech()        const;
    bool    isBuilding()    const;

    const size_t &          getMetaType()  const;
    const std::string &     getName()       const;
    const CCRace &          getRace()       const;
    const UnitType &        getUnitType() const;
    const CCUpgrade &       getUpgrade()  const;

    std::vector<UnitType>   whatBuilds;
};

