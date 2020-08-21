#pragma once
#include <sc2api/sc2_api.h>

class TestAgent : public sc2::Agent {
 private:
  bool lostTheGame = false;

 public:

  void OnGameEnd() override;

  bool isLoss();
};


