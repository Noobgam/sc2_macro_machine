#pragma once

#include "../../general/model/Unit.h"

typedef long long SquadID;

class SquadManager;

class Squad {
private:
    friend SquadManager;
    std::set<Unit> m_units;
    const SquadID m_id;

    void addUnits(const std::set<Unit> & units);
    void removeUnits(const std::set<Unit> & units);
    void clear();
public:
    Squad(SquadID id);

    const SquadID getId() const;

    const bool isEmpty() const;
    const std::set<Unit> & units() const;
};