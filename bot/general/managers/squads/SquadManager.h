#pragma once

#include "../../../micro/squads/Squad.h"
#include "../../model/Common.h"

class CCBot;

class SquadManager {
private:
    CCBot & m_bot;

    SquadID currentSquadID = 0;
    std::map<SquadID, std::unique_ptr<Squad>> m_squads;
    std::map<CCUnitID, Squad*> m_units;

    void removeUnitsFromSquad(const std::set<const Unit*> & units, Squad* squad);
public:
    SquadID unassignedSquadID;

    explicit SquadManager(CCBot & bot);

    void onFrame();

    Squad* getUnassignedSquad() const;
    Squad* getUnitSquad(const Unit* unit) const;
    std::optional<Squad*> getSquad(SquadID id) const;

    void addUnitCallback(const Unit* unit);
    void removeUnitCallback(const Unit* unit);

    Squad* createNewSquad();
    Squad* mergeSquads(std::vector<Squad*> & squads);

    void transferUnits(Squad* from, Squad* to);
    void transferUnits(const std::set<const Unit*> & units, Squad* to);
};
