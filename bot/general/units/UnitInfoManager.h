#pragma once

#include "../model/Common.h"
#include "../model/Unit.h"
#include <general/influence/ThreatEstimatingInfluenceMap.h>

class CCBot;

enum UnitStatus {
    COMPLETED   = 0x01,
    CONSTRUCTING    = 0x02,
    ORDERED     = 0x04,

    NOT_TRAINING = 0x08,

    IN_PROGRESS = CONSTRUCTING | ORDERED,
    TOTAL = CONSTRUCTING | ORDERED | COMPLETED,
    FREE = COMPLETED | NOT_TRAINING,
};

class UnitInfoManager {
    CCBot &           m_bot;

    std::unordered_map<sc2::Tag, std::unique_ptr<Unit>> unitWrapperByTag;
    std::map<CCPlayer, std::vector<const Unit*>> m_units;
    ThreatEstimatingInfluenceMap globalInfluenceMap;

    void                    updateUnits();

    void processNewUnit(const Unit *unit);
    void updateSquadsWithNewUnit(const Unit *unit);

    void processRemoveUnit(const Unit *unit);
    void updateSquadsWithRemovedUnit(const Unit *unit);
    void drawUnits();

public:
    explicit UnitInfoManager(CCBot & bot);

    void                    onFrame();
    void                    onStart();

    const std::vector<const Unit*> & getUnits(CCPlayer player) const;
    std::vector<const Unit*> getUnits(CCPlayer player, sc2::UnitTypeID type) const;
    size_t                  getUnitTypeCount(CCPlayer player, UnitType type, bool completed = true) const;
    int                     getBuildingCount(CCPlayer player, UnitType type, UnitStatus status) const;
};