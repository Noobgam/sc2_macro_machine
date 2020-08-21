#include "TestRunner.h"
TestRunner::TestRunner(const TestScenario &scenario)
    : scenario(scenario)
{
    testedAgent = std::move(scenario.testedAgentFactory());
    testScenarioAgent = std::move(scenario.testScenarioAgentFactory());
}

void TestRunner::run(int argc, char** argv) {
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
    while (coordinator.Update());
    assert(testScenarioAgent.get()->isLoss());
}
