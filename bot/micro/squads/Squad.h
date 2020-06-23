#pragma once

#include "../../general/model/Unit.h"

typedef unsigned long long SquadID;

class SquadManager;

class Squad {
private:
    friend SquadManager;
    std::set<std::shared_ptr<Unit>> m_units;
    const SquadID m_id;

    void addUnits(const std::set<std::shared_ptr<Unit>> & units);
    void removeUnits(const std::set<std::shared_ptr<Unit>> & units);
    void clear();
public:
    Squad(SquadID id);

    const SquadID getId() const;

    const bool isEmpty() const;
    const std::set<std::shared_ptr<Unit>> & units() const;
};