#pragma once

class CCBot;
class BaseLocation;
#include <micro/squads/Squad.h>
#include <cannons/BaseAnalysis.h>
#include <future>
#include <cannons/BasicAnalyser.h>
#include <strategy/Strategy.h>

// this module is responsible for first couple of probes (before cannons are built, mostly about pylon placements)
// the calculations inside this module must be real-time in regards to "not calculated statically"
// but they have to be done asynchronously, because they might take a lot of time
class CannonStartModule {
private:
    CCBot & m_bot;
    std::optional<SquadID> m_mainSquad;
    std::optional<SquadID> m_subSquad;
    BasicAnalyser analyzer;
    std::unique_ptr<BaseAnalysis> currentAnalysis = NULL;
    std::optional<PylonPlacement> selectedPlacement;
    int latestProcessedRevision = -1;
    bool needRecalculation = false;
    Strategy::HighLevelStrategy strategy = Strategy::HighLevelStrategy::NONE;
    std::optional<Squad*> assignScoutSquad(const BaseLocation* baseLocation);
    void updateStrategy();
    bool isPylonPlacementScary(const PylonPlacement& pylonPlacement) const;
    bool isPlacementValid(const PylonPlacement& placement) const;
public:
    explicit CannonStartModule(CCBot & bot);

    void onFrame();
    void newUnitCallback(const Unit *unit);
};


