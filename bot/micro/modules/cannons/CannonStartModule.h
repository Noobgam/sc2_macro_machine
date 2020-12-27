#pragma once

class CCBot;
class BaseLocation;
#include <micro/squads/Squad.h>
#include <cannons/BaseAnalysis.h>
#include <future>
#include <cannons/BasicAnalyser.h>

// this module is responsible for first couple of probes (before cannons are built, mostly about pylon placements)
// the calculations inside this module must be real-time in regards to "not calculated statically"
// but they have to be done asynchronously, because they might take a lot of time
class CannonStartModule {
private:
    CCBot & m_bot;
    std::optional<SquadID> m_mainScoutID;
    std::optional<SquadID> m_subScoutID;
    BasicAnalyser analyzer;
    BaseAnalysis* currentAnalysis = NULL;
    bool needRecalculation = false;

    std::optional<Squad*> assignScoutSquad(const BaseLocation* baseLocation);
public:
    explicit CannonStartModule(CCBot & bot);

    void onFrame();
    void newUnitCallback(const Unit *unit);
};


