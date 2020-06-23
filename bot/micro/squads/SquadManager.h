#pragma once

#include "Squad.h"
#include "../../general/model/Common.h"

class SquadManager {
private:
    static SquadID currentSquadID;
    std::map<SquadID, std::shared_ptr<Squad>> m_squads;
public:
    SquadID unassignedSquadID;

    SquadManager();
    const std::map<SquadID, std::shared_ptr<Squad>> & getSquads() const;

    const Squad & getUnassignedSquad() const;
    std::optional<std::shared_ptr<Squad>> getSquad(SquadID) const;

    SquadID createNewSquad();
    SquadID mergeSquads(const std::vector<SquadID> & ids);

    void transferUnits(SquadID from, SquadID to);
    void transferUnits(const std::set<std::shared_ptr<Unit>> & units, SquadID to);
};
