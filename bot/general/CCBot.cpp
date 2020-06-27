#include "CCBot.h"
#include "../util/LogInfo.h"

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
    LOG_DEBUG << "Starting OnGameStart()" << std::endl;
    for (auto & loc : Observation()->GetGameInfo().enemy_start_locations)
    {
        m_baseLocations.push_back(loc);
    }
    m_baseLocations.emplace_back(Observation()->GetStartLocation());
    
    setUnits();
    m_techTree.onStart();
    m_map.onStart();
    m_unitInfo.onStart();
    m_bases.onStart();
    m_workers.onStart();

    m_gameCommander.onStart();
    LOG_DEBUG << "Finished OnGameStart()" << std::endl;
}

void CCBot::OnGameFullStart() {
    LOG_DEBUG << "Starting OnGameFullStart()" << std::endl;
    LOG_DEBUG << "Finished OnGameFullStart()" << std::endl;
}

void CCBot::OnGameEnd() {
    LOG_DEBUG << "Starting OnGameEnd()" << std::endl;
    LOG_DEBUG << "Finished OnGameEnd()" << std::endl;
}

void CCBot::OnStep()
{
    LOG_DEBUG << "Starting onStep()" << std::endl;
    setUnits();
    m_map.onFrame();
    m_unitInfo.onFrame();
    m_bases.onFrame();
    m_workers.onFrame();

    m_gameCommander.onFrame();
    LOG_DEBUG << "Finished onStep()" << std::endl;

#ifdef _DEBUG
    Debug()->SendDebug();
#endif
}

void CCBot::setUnits()
{
    Control()->GetObservation();
    ++observationId;
    for (auto & unit : Observation()->GetUnits())
    {
        auto it = unitWrapperByTag.find(unit->tag);
        if (it == unitWrapperByTag.end()) {
            unitWrapperByTag.insert({unit->tag, std::make_unique<Unit>(unit, *this, observationId)});
        } else {
            it->second->updateUnit(unit, observationId);
        }
    }
    // mostly cleanups dead units
    std::vector<std::unique_ptr<Unit>> missingUnits;
    for (auto it = unitWrapperByTag.begin(); it != unitWrapperByTag.end(); ) {
        if (it->second->getObservationId() != observationId) {
            std::unique_ptr<Unit> pt = std::move(it->second);
            //missingUnits.push_back(std::move(it->second));
            it = unitWrapperByTag.erase(it);
        } else {
            ++it;
        }
    }

    // callback missingUnits before destruction

    m_allUnits.clear();
    m_allUnits.reserve(unitWrapperByTag.size());
    for (const auto & it : unitWrapperByTag) {
        m_allUnits.push_back(it.second.get());
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

const std::vector<Unit*>& CCBot::GetUnits() const
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
    auto& logger = LOG_DEBUG;
    if (client_errors.size() > 0) {
        logger << "Critical client errors occured in sc2:" << std::endl;
        logger << "\t";
        for (const auto& clErr : client_errors) {
             logger << static_cast<int>(clErr) << " ";
        }
        logger << std::endl;
    }
    if (protocol_errors.size() > 0) {
        logger << "Critical protocol errors occured in sc2:" << std::endl;
        for (const auto& clErr : protocol_errors) {
            logger << "\t" << clErr << std::endl;
        }
    }
}

UnitType CCBot::getUnitType(sc2::UnitTypeID typeId) {
    return UnitType{ typeId, *this };
}