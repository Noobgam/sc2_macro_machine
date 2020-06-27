#pragma once

#include "Squad.h"
#include "../../general/model/Common.h"

class SquadManager {
private:
    SquadID currentSquadID = 0;
    std::map<SquadID, std::unique_ptr<Squad>> m_squads;
    std::map<Unit*, Squad*> m_units;

    void removeUnitsFromSquad(const std::set<Unit*> & units, Squad* squad);
public:
    SquadID unassignedSquadID;

    SquadManager();

    Squad* getUnassignedSquad() const;
    Squad* getUnitSquad(Unit* unit) const;
    std::optional<Squad*> getSquad(SquadID) const;

    void addUnit(Unit* unit);
    void removeUnit(Unit* unit);

    Squad* createNewSquad();
    Squad* mergeSquads(std::vector<Squad*> & ids);

    void transferUnits(Squad* from, Squad* to);
    void transferUnits(const std::set<Unit*> & units, Squad* to);
};
