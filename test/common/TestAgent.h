#pragma once
#include <sc2api/sc2_api.h>
#include "ScenarioResult.h"

class TestScenario;

class TestAgent : public sc2::Agent {
  friend class TestRunner;

 protected:
  ScenarioResult scenarioResult = ScenarioResult::NONE;

  // You can take a look at the scenario if necessary
  // Both scenario and enemyAgent only exist after they are bound by TestRunner::run
  TestScenario* scenario = nullptr;

  // Please do not use internals of the agent by casting it to specific type.
  // It is very unsafe and is subject to change in the future.
  sc2::Agent* enemyAgent = nullptr;

 public:

  void OnGameEnd() override;
};


