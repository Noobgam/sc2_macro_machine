#pragma once

#include <vector>

struct GraphCheckResult {
    std::vector<std::pair<int,int>> articulationPoints;
    std::vector<std::pair<std::pair<int,int>, std::pair<int,int>>> bridges;
};


