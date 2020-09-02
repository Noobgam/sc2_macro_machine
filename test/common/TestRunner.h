#pragma once

#include <general/CCBot.h>
#include "TestScenario.h"

class TestRunner {
    std::unique_ptr<sc2::Agent> testedAgent;
    std::unique_ptr<TestAgent> testScenarioAgent;
    TestScenario scenario;

 public:
  TestRunner(const TestScenario &scenario);
  void run(int argc, char** argv);
};


