#pragma once

#include "model/Common.h"
#include "UnitData.h"
#include "BaseLocation.h"
#include "model/Unit.h"

enum UnitStatus {
    COMPLETED   = 0x01,
    CONSTRUCTING    = 0x02,
    ORDERED     = 0x04,

    NOT_TRAINING = 0x08,

    IN_PROGRESS = CONSTRUCTING | ORDERED,
    TOTAL = CONSTRUCTING | ORDERED | COMPLETED,
    FREE = COMPLETED | NOT_TRAINING,
};

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

    int                     getBuildingCount(CCPlayer player, UnitType type, UnitStatus status) const;

    void                    getNearbyForce(std::vector<UnitInfo> & unitInfo, CCPosition p, int player, float radius) const;

    const std::map<Unit, UnitInfo> & getUnitInfoMap(CCPlayer player) const;

    void                    drawUnitInformation(float x, float y) const;

};