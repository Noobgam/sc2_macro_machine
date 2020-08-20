#pragma once

#include <general/model/Unit.h>
#include <general/bases/BaseLocation.h>
#include "Base.h"

class CCBot;

class BasesManager {
private:
    CCBot & m_bot;

    std::vector<std::unique_ptr<Base>> m_bases = {};
    std::vector<Base*> m_basesPtrs = {};
    Base* m_startLocation = nullptr;

    void newBaseOccupied(const BaseLocation* baseLocation, const Unit* nexus);
    void tryAddAssimilator(Base * base, const Unit * unit);
public:
    explicit BasesManager(CCBot & bot);

    void onStart();

    void newUnitCallback(const Unit* unit);
    void unitDisappearedCallback(const Unit* unit);

    const std::vector<Base*> & getBases() const;
    std::vector<Base*> getCompletedBases() const;
    bool isBaseOccupied(BaseLocationID baseId) const;

    const Base* getStartLocation() const;
};
