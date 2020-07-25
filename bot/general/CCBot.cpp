#include <general/map_meta/wall/WallPlacement.h>
#include <util/LogInfo.h>
#include "CCBot.h"
#include <random>
#include <ctime>

CCBot::CCBot()
    : m_map(*this)
    , m_bases(*this)
    , m_unitInfo(*this)
    , m_managers(*this)
    , m_gameCommander(*this)
    , m_techTree(*this)
{ }

void CCBot::OnGameStart() {
    m_techTree.onStart();
#ifdef _STATIC_MAP_CALCULATOR
    StaticMapMeta::getMeta(*this);
    // reload it to validate
    StaticMapMeta::getMeta(*this);
    std::terminate();
#else
    m_mapMeta = MapMeta::getMeta(*this);
#endif

    LOG_DEBUG << "Starting OnGameStart()" << std::endl;

    m_unitInfo.onStart();

    m_map.onStart();
    m_managers.onStart();


    m_gameCommander.onStart();
    int myBaseId = (*m_bases.getOccupiedBaseLocations(Players::Self).begin())->getBaseId();
    m_wallPlacements = m_mapMeta->getWallPlacements(myBaseId, myBaseId);
    if (m_wallPlacements.size() != 0) {
        while (true) {
            chosenPlacement = m_wallPlacements[rand() % m_wallPlacements.size()];
            if (chosenPlacement.value().wallType != WallType::FullWall) break;
        }
        for (auto x : chosenPlacement->gaps) {
            if (x.second == GapType::OneByOne) {
                m_wallCandidates.push_back({x.first.first, x.first.second});
            }
        }
    } else {
//        m_wallCandidates = WallPlacement::getTileCandidates(*this,
//                myBaseId,
//                enemyBaseId
//                );
    }
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
    m_unitInfo.onFrame();

    m_map.onFrame();
    m_bases.onFrame();

    m_managers.onFrame();

    m_gameCommander.onFrame();
    LOG_DEBUG << "Finished onStep()" << std::endl;

#ifdef _DEBUG
    if (chosenPlacement.has_value()) {
        for (auto x : chosenPlacement->buildings) {
            int sz = 3;
            if (x.second == BuildingType::PoweringPylon) {
                sz = 2;
            }
            Map().drawText({x.first.first + .0f, x.first.second + .0f}, "Wall part");
            for (int i = 0; i < sz; ++i) {
                for (int j = 0; j < sz; ++j) {
                    Map().drawTile(i + x.first.first, j + x.first.second);
                }
            }
        }

        for (auto tile : m_wallCandidates) {
            Map().drawTile(tile.x, tile.y, CCColor(0, 255, 0));
        }
    } else {
        for (auto x : m_wallCandidates) {
            Map().drawTile(x.x, x.y);
        }
    }
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

const MapMeta & CCBot::MapMeta() const {
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

bool CCBot::NeedWall() const {
    return true;
}
