#include <util/LogInfo.h>
#include "TestRunner.h"
#include "TestAgent.h"

TestRunner::TestRunner(const TestScenario &scenario)
    : scenario(scenario)
{
    testedAgent = std::move(scenario.testedAgentFactory());
    testScenarioAgent = std::move(scenario.testScenarioAgentFactory());
}

void TestRunner::run(int argc, char** argv) {
    LOG_DEBUG << "Executing scenario [" << scenario.name << "]" << std::endl;
    // bind test scenario agent before starting the game
    testScenarioAgent->scenario = &scenario;
    testScenarioAgent->enemyAgent = testedAgent.get();

    sc2::Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);
    coordinator.SetStepSize(1);
    coordinator.SetRealtime(scenario.realtime);
    coordinator.SetRandomSeed({scenario.seed});

    coordinator.SetParticipants({
        sc2::CreateParticipant(scenario.testScenarioAgentRace, testScenarioAgent.get()),
        sc2::CreateParticipant(scenario.testedAgentRace, testedAgent.get())
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(scenario.mapString);

    // Step forward the game simulation.
    while (coordinator.Update() && testScenarioAgent->scenarioResult == ScenarioResult::NONE);
    if (testScenarioAgent->scenarioResult != ScenarioResult::SUCCESS) {
        testScenarioAgent->Control()->SaveReplay(scenario.name);
    }
}
