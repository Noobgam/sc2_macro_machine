#pragma once

#include "../general/model/Common.h"

class CCBot;
class Unit;
class Resource;
class UnitType;

#ifdef _STATIC_MAP_CALCULATOR
#define VALIDATE_CALLED_ONCE() ((void)0)
#else
#define VALIDATE_CALLED_ONCE() \
    static bool alreadyCalled = false; \
    BOT_ASSERT(!alreadyCalled, "This function can should only be called once.)"); \
    alreadyCalled = true
#endif

namespace Util
{
    CCRace          GetRaceFromString(const std::string & str);
    CCTilePosition  GetTilePosition(const CCPosition & pos);
    CCPosition      GetPosition(const CCTilePosition & tile);
    std::string     GetStringFromRace(const CCRace & race);
    bool            UnitCanMetaTypeNow(const Unit & unit, const UnitType & type, CCBot & m_bot);
    UnitType        GetTownHall(const CCRace & race, CCBot & bot);
    UnitType        GetRefinery(const CCRace & race, CCBot & bot);
    UnitType        GetSupplyProvider(const CCRace & race, CCBot & bot);
    CCPosition      CalcCenter(const std::vector<const Resource*> & minerals);
    CCPosition      CalcCenter(const std::vector<const sc2::Unit*> & minerals);
    CCPosition      CalcCenter(const std::vector<const Unit*> & minerals);
    CCPosition      NormalizeVector(const CCPosition& pos);
    bool            IsZerg(const CCRace & race);
    bool            IsProtoss(const CCRace & race);
    bool            IsTerran(const CCRace & race);
    CCPositionType  TileToPosition(float tile);
    bool            isRound(float f);

#ifdef SC2API
    sc2::BuffID     GetBuffFromName(const std::string & name, CCBot & bot);
    sc2::AbilityID  GetAbilityFromName(const std::string & name, CCBot & bot);
#endif

    float Dist(const Unit & unit, const CCPosition & p2);
    float Dist(const Unit* unit, const CCPosition & p2);
    float Dist(const Unit & unit1, const Unit & unit2);
    float Dist(const CCPosition & p1, const CCPosition & p2);
    CCPositionType DistSq(const CCPosition & p1, const CCPosition & p2);
    bool canWalkOverUnit(const UnitType& type);
    bool canBuildOnUnit(const UnitType& type);
};
