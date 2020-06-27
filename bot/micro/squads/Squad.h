#pragma once

#include "../../general/model/Unit.h"

typedef unsigned long long SquadID;

class SquadManager;

class Squad {
private:
    friend SquadManager;
    std::set<const Unit*> m_units;
    const SquadID m_id;

    void addUnits(const std::set<const Unit*> & units);
    void removeUnits(const std::set<const Unit*> & units);
    void clear();
public:
    Squad(SquadID id);

    SquadID getId() const;

    bool isEmpty() const;
    const std::set<const Unit*> & units() const;
};