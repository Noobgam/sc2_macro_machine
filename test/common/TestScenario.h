#pragma once

#include <string>
#include <memory>
#include "TestAgent.h"

struct TestScenario {
  // name to distinguish scenarios.
  std::string name;

  std::string mapString;
  int seed;
  bool realtime;
  sc2::Race testedAgentRace;
  sc2::Race testScenarioAgentRace;
  std::function<std::unique_ptr<sc2::Agent>()> testedAgentFactory;
  std::function<std::unique_ptr<TestAgent>()> testScenarioAgentFactory;

  TestScenario(const std::string &name,
               const std::string &map_string,
               int seed,
               bool realtime,
               sc2::Race tested_agent_race,
               sc2::Race test_scenario_agent_race,
               const std::function<std::unique_ptr<sc2::Agent>()> &tested_agent_factory,
               const std::function<std::unique_ptr<TestAgent>()> &test_scenario_agent_factory)
      : name(name),
        mapString(map_string),
        seed(seed),
        realtime(realtime),
        testedAgentRace(tested_agent_race),
        testScenarioAgentRace(test_scenario_agent_race),
        testedAgentFactory(tested_agent_factory),
        testScenarioAgentFactory(test_scenario_agent_factory) {}
};