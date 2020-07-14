#pragma once

#include <sc2api/sc2_api.h>
#include "MapTools.h"
#include "bases/BaseLocationManager.h"
#include "units/UnitInfoManager.h"
#include "TechTree.h"
#include "../GameCommander.h"
#include "managers/Managers.h"
#include "general/map_meta/WallPlacement.h"

class CCBot : public sc2::Agent {
    MapTools                m_map;
    BaseLocationManager     m_bases;
    UnitInfoManager         m_unitInfo;
    TechTree                m_techTree;
    GameCommander           m_gameCommander;
    Managers                m_managers;


    std::unique_ptr<MapMeta>        m_mapMeta;          // static map information (e.g. wall placements, scouting waypoints)


    size_t observationId = 0;

    void OnError(const std::vector<sc2::ClientError> & client_errors, 
                 const std::vector<std::string> & protocol_errors = {}) override;

public:
    // TODO: move to mapMeta
    std::vector<WallPlacement>      m_wallPlacements;
    WallPlacement chosenPlacement;

    CCBot();

    void OnGameFullStart() override;
    void OnGameStart() override;
    void OnGameEnd() override;
    void OnStep() override;

    size_t getObservationId() const;

    const BaseLocationManager & Bases() const;
    const MapTools & Map() const;
    const UnitInfoManager & UnitInfo() const;
          GameCommander & Commander();
          Managers & getManagers();

    const TypeData & Data(const UnitType & type) const;
    const TypeData & Data(const CCUpgrade & type) const;
    const TypeData & Data(const MetaType & type) const;
    const TypeData & Data(const Unit & unit) const;
    CCRace GetPlayerRace(int player) const;
    CCPosition GetStartLocation() const;
    UnitType getUnitType(sc2::UnitTypeID typeId);

    int GetCurrentFrame() const;
    int GetMinerals() const;
    int GetCurrentSupply() const;
    int GetMaxSupply() const;
    int GetGas() const;
    bool NeedWall() const;
};