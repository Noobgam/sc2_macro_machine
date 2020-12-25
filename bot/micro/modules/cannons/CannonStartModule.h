#pragma once

class CCBot;
#include <micro/squads/Squad.h>
#include <cannons/BaseAnalysis.h>

// this module is responsible for first couple of probes (before cannons are built, mostly about pylon placements)
// the calculations inside this module must be real-time in regards to "not calculated statically"
// but they have to be done asynchronously, because they might take a lot of time
class CannonStartModule {
private:
    CCBot & m_bot;
    std::optional<SquadID> m_mainScoutID;
    std::atomic<BaseAnalysis*> latestAnalysis = NULL;
public:
    explicit CannonStartModule(CCBot & bot);

    void onFrame();
};


