#include "CCBot.h"
#include "../util/LogInfo.h"

CCBot::CCBot()
    : m_map(*this)
    , m_bases(*this)
    , m_unitInfo(*this)
    , m_workers(*this)
    , m_managers(*this)
    , m_gameCommander(*this)
    , m_techTree(*this)
{ }

void CCBot::OnGameStart() {
    LOG_DEBUG << "Starting OnGameStart()" << std::endl;

    m_techTree.onStart();
    m_map.onStart();
    m_unitInfo.onStart();
    m_bases.onStart();
    m_workers.onStart();

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
    LOG_DEBUG << "Starting onStep()" << std::endl;
    Control()->GetObservation();
    ++observationId;
    m_map.onFrame();
    m_unitInfo.onFrame();
    m_bases.onFrame();
//    m_workers.onFrame();

    m_managers.onFrame();

    m_gameCommander.onFrame();
    LOG_DEBUG << "Finished onStep()" << std::endl;

#ifdef _DEBUG
    Debug()->SendDebug();
#endif
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

WorkerManager_L & CCBot::Workers()
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

bool CCBot::NeedWall() const {
    // TODO: return true if wall is actually needed
    return false;
}
