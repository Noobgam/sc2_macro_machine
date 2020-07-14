#pragma once

#include "WallPlacement.h"
#include <optional>
#include <vector>
class CCBot;

// verifies wall placements for specific base
struct WallVerifier {
    WallVerifier(
            const CCBot& bot,
            int baseLocationId,
            int startBaseLocationId,
            int enemyStartBaseLocationId
            );

    std::optional<WallPlacement> verifyPlacement(
            // {{lx, ly}, type} - position of a left-bottom most tile, and building size
            const std::vector<std::pair<std::pair<int,int>, BuildingType>>& buildings
    );
private:
    const CCBot& m_bot;
    int m_baseLocationId;
    int m_startBaseLocationId;
    int m_enemyStartBaseLocationId;
};


