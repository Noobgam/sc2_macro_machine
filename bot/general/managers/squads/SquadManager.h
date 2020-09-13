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

    // Beware: there may be stray shades which are not present in this map
    // during unit removal procedure
    std::vector<std::pair<const Unit*, const Unit*>> adeptsAndShades;

public:
    SquadID unassignedSquadID;

    explicit SquadManager(CCBot & bot);

    void onStart();
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

    void deformSquad(Squad* squad);
};
