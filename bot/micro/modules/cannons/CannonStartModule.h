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
    std::optional<Squad*> m_mainSquad;
    std::optional<Squad*> m_subSquad;
    BasicAnalyser analyzer;
    BaseAnalysis* currentAnalysis = NULL;
    std::optional<PylonPlacement> selectedPlacement;
    bool needRecalculation = false;
    std::optional<Squad*> assignScoutSquad(const BaseLocation* baseLocation);
public:
    explicit CannonStartModule(CCBot & bot);

    void onFrame();
    void newUnitCallback(const Unit *unit);
};


