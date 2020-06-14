#include "CCBot.h"
#include "../Util.h"

CCBot::CCBot()
    : m_map(*this)
    , m_bases(*this)
    , m_unitInfo(*this)
    , m_workers(*this)
    , m_gameCommander(*this)
    , m_techTree(*this)
{

}

void CCBot::OnGameStart() 
{
    for (auto & loc : Observation()->GetGameInfo().enemy_start_locations)
    {
        m_baseLocations.push_back(loc);
    }
    m_baseLocations.push_back(Observation()->GetStartLocation());
    
    setUnits();
    m_techTree.onStart();
    m_map.onStart();
    m_unitInfo.onStart();
    m_bases.onStart();
    m_workers.onStart();

    m_gameCommander.onStart();
}

void CCBot::OnStep()
{
    setUnits();
    m_map.onFrame();
    m_unitInfo.onFrame();
    m_bases.onFrame();
    m_workers.onFrame();

    m_gameCommander.onFrame();

    Debug()->SendDebug();
}

void CCBot::setUnits()
{
    m_allUnits.clear();
    Control()->GetObservation();
    for (auto & unit : Observation()->GetUnits())
    {
        m_allUnits.push_back(Unit(unit, *this));    
    }
}

CCRace CCBot::GetPlayerRace(int player) const
{
    auto playerID = Observation()->GetPlayerID();
    for (auto & playerInfo : Observation()->GetGameInfo().player_info)
    {
        if (playerInfo.player_id == playerID)
        {
            return playerInfo.race_actual;
        }
    }

    BOT_ASSERT(false, "Didn't find player to get their race");
    return sc2::Race::Random;
}

const MapTools & CCBot::Map() const
{
    return m_map;
}

const BaseLocationManager & CCBot::Bases() const
{
    return m_bases;
}

const UnitInfoManager & CCBot::UnitInfo() const
{
    return m_unitInfo;
}

int CCBot::GetCurrentFrame() const
{
    return (int)Observation()->GetGameLoop();
}

const TypeData & CCBot::Data(const UnitType & type) const
{
    return m_techTree.getData(type);
}

const TypeData & CCBot::Data(const Unit & unit) const
{
    return m_techTree.getData(unit.getType());
}

const TypeData & CCBot::Data(const CCUpgrade & type) const
{
    return m_techTree.getData(type);
}

const TypeData & CCBot::Data(const MetaType & type) const
{
    return m_techTree.getData(type);
}

WorkerManager & CCBot::Workers()
{
    return m_workers;
}

int CCBot::GetCurrentSupply() const
{
    return Observation()->GetFoodUsed();
}

int CCBot::GetMaxSupply() const
{
    return Observation()->GetFoodCap();
}

int CCBot::GetMinerals() const
{
    return Observation()->GetMinerals();
}

int CCBot::GetGas() const
{
    return Observation()->GetVespene();
}

Unit CCBot::GetUnit(const CCUnitID & tag) const
{
    return Unit(Observation()->GetUnit(tag), *(CCBot *)this);
}

const std::vector<Unit> & CCBot::GetUnits() const
{
    return m_allUnits;
}

CCPosition CCBot::GetStartLocation() const
{
    return Observation()->GetStartLocation();
}

const std::vector<CCPosition> & CCBot::GetStartLocations() const
{
    return m_baseLocations;
}

void CCBot::OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors)
{
    
}

UnitType CCBot::getUnitType(sc2::UnitTypeID typeId) {
    return UnitType{ typeId, *this };
}