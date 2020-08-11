#include <general/map_meta/wall/WallPlacement.h>
#include <util/LogInfo.h>
#include "CCBot.h"
#include <random>
#include <ctime>

#include <thread>
#include <chrono>

CCBot::CCBot()
    : m_map(*this)
    , m_bases(*this)
    , m_unitInfo(*this)
    , m_managers(*this)
    , m_gameCommander(*this)
    , m_techTree(*this)
    , m_unitCommandManager(*this)
{ }

void CCBot::OnGameStart() {
    m_techTree.onStart();
    m_unitInfo.onStart();

#ifdef _STATIC_MAP_CALCULATOR
    return;
#endif


    LOG_DEBUG << "Starting OnGameStart()" << std::endl;

    m_map.onStart();
    m_managers.onStart();


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

void CCBot::OnStep() {
    logging::propagateFrame(GetCurrentFrame());
    ++observationId;
#ifdef _STATIC_MAP_CALCULATOR
    if (observationId == 1) {
        Debug()->DebugShowMap();
        Debug()->SendDebug();
        return;
    }
    StaticMapMeta::getMeta(*this);
    // reload it to validate
    StaticMapMeta::getMeta(*this);
    Control()->RequestLeaveGame();
    return;
#endif
    Timer timer;
    timer.start();
    LOG_DEBUG << "Starting onStep()" << std::endl;
    m_unitInfo.onFrame();

    m_map.onFrame();
    m_bases.onFrame();

    m_managers.onFrame();

    m_gameCommander.onFrame();

#ifdef _DEBUG
    Debug()->SendDebug();
#endif

    m_unitCommandManager.issueAllCommands(GetCurrentFrame());
    timer.stop();
    LOG_DEBUG << "Finished onStep(): " << timer.getElapsedTimeInMilliSec() << "ms" << std::endl;
}

size_t CCBot::getObservationId() const {
    return observationId;
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

const MapTools & CCBot::Map() const {
    return m_map;
}

const MapMeta & CCBot::getMapMeta() const {
    return *m_mapMeta.get();
}

BaseLocationManager & CCBot::Bases() {
    return m_bases;
}

const BaseLocationManager & CCBot::Bases() const {
    return m_bases;
}

const UnitInfoManager & CCBot::UnitInfo() const {
    return m_unitInfo;
}

GameCommander &CCBot::Commander() {
    return m_gameCommander;
}

Managers &CCBot::getManagers() {
    return m_managers;
}

const Managers &CCBot::getManagers() const {
    return m_managers;
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

//const std::vector<const sc2::Unit*>& CCBot::GetUnits() const {
//    return Observation()->GetUnits();
//}

CCPosition CCBot::GetStartLocation() const
{
    return Observation()->GetStartLocation();
}

void CCBot::OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors) {
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

UnitCommandManager &CCBot::getUnitCommandManager() {
    return m_unitCommandManager;
}
