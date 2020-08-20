#pragma once

#include <micro/squads/Squad.h>

class CCBot;

class ScoutModule {
private:
    CCBot & m_bot;
    std::optional<SquadID> m_basesScoutID;
public:
    explicit ScoutModule(CCBot & bot);

    void onFrame();
};
