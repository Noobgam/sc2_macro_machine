#pragma once

#include "model/Common.h"
#include "../util/util.h"

#include "../BotConfig.h"
#include "../GameCommander.h"
#include "MapTools.h"
#include "BaseLocationManager.h"
#include "units/UnitInfoManager.h"
#include "WorkerManager.h"
#include "TechTree.h"
#include "model/MetaType.h"
#include "model/Unit.h"

class CCBot : public sc2::Agent 
{
    MapTools                m_map;
    BaseLocationManager     m_bases;
    UnitInfoManager         m_unitInfo;
    WorkerManager           m_workers;
    TechTree                m_techTree;
    GameCommander           m_gameCommander;

    std::vector<CCPosition> m_baseLocations;

    size_t observationId = 0;

    void OnError(const std::vector<sc2::ClientError> & client_errors, 
                 const std::vector<std::string> & protocol_errors = {}) override;

public:

    CCBot();

    void OnGameFullStart() override;
    void OnGameStart() override;
    void OnGameEnd() override;
    void OnStep() override;

    size_t getObservationId() const;

          WorkerManager & Workers();
    const BaseLocationManager & Bases() const;
    const MapTools & Map() const;
    const UnitInfoManager & UnitInfo() const;
          GameCommander & Commander();

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
    const std::vector<const sc2::Unit*> & GetUnits() const;
    const std::vector<CCPosition> & GetStartLocations() const;
};