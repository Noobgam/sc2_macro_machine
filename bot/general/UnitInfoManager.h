#pragma once

#include "model/Common.h"
#include "UnitData.h"
#include "BaseLocation.h"
#include "model/Unit.h"

class CCBot;
class UnitInfoManager 
{
    CCBot &           m_bot;

    std::map<CCPlayer, UnitData> m_unitData; 
    std::map<CCPlayer, std::vector<Unit>> m_units;

    void                    updateUnit(const Unit & unit);
    void                    updateUnitInfo();
    bool                    isValidUnit(const Unit & unit);
    
    const UnitData &        getUnitData(CCPlayer player) const;

    void drawSelectedUnitDebugInfo();

public:

    UnitInfoManager(CCBot & bot);

    void                    onFrame();
    void                    onStart();

    const std::vector<Unit> & getUnits(CCPlayer player) const;

    size_t                  getUnitTypeCount(CCPlayer player, UnitType type, bool completed = true) const;

    int                     getBuildingCount(CCPlayer player, UnitType type, uint8_t status) const;

    void                    getNearbyForce(std::vector<UnitInfo> & unitInfo, CCPosition p, int player, float radius) const;

    const std::map<Unit, UnitInfo> & getUnitInfoMap(CCPlayer player) const;

    void                    drawUnitInformation(float x, float y) const;

};

struct UnitStatus {
    static const uint8_t COMPLETED   = 0x01;
    static const uint8_t BUILDING    = 0x02;
    static const uint8_t ORDERED     = 0x04;

    static const uint8_t IN_PROGRESS = BUILDING | ORDERED;
};