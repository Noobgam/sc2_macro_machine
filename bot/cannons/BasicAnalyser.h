#pragma once

#include <vector>
#include <future>
#include <general/bases/BaseLocation.h>
#include "BaseAnalysis.h"

class CCBot;

class BasicAnalyser {
private:
public:
    std::vector<std::vector<bool>> walkable;
    std::vector<std::vector<bool>> buildable;

    // geysers should be considered the same as walls, but minerals cannot be walked diagonally (except for pylon trick)
    std::vector<std::vector<bool>> minerals;

    std::atomic<BaseAnalysis*> latestAnalysis = NULL;
    std::future<void> lastCalculationFuture;

    CCTilePosition tilePosition;

public:
    void recalculate(const CCBot& bot);
    bool analysisReady();
    bool analysisInProgress();

    void analyze(const BaseLocation* baseLocation);
};


