#include "TestAgent.h"
#include "TestScenario.h"
#include <util/LogInfo.h>

void TestAgent::OnGameEnd() {
    Control()->GetObservation();
    auto res = Observation()->GetResults();
    auto myId = Observation()->GetPlayerID();
    auto it = std::find_if(res.begin(), res.end(), [myId] (const sc2::PlayerResult& pr) {
      return pr.player_id == myId;
    });
    if (it != res.end() && it->result == sc2::GameResult::Loss) {
        scenarioResult = ScenarioResult::SUCCESS;
    }
}

void TestAgent::OnStep() {
    if (logging::getPropagatedFrame() > 22.4 * 10 * 60) {
        // TODO: save replay #147
        //Control()->SaveReplay("some_path");
        exit(1);
    }
}
