#pragma once

#include "Squad.h"
#include "../../general/model/Common.h"

class SquadManager {
private:
    static SquadID currentSquadID;
    std::map<SquadID, std::unique_ptr<Squad>> m_squads;
public:
    SquadID unassignedSquadID;

    SquadManager();
//    const std::map<SquadID, Squad*> & getSquads() const;

    const Squad & getUnassignedSquad() const;
    std::optional<Squad*> getSquad(SquadID) const;

    SquadID createNewSquad();
    SquadID mergeSquads(const std::vector<SquadID> & ids);

    void transferUnits(SquadID from, SquadID to);
    void transferUnits(const std::set<Unit*> & units, SquadID to);
};
