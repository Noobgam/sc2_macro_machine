#pragma once

#include "WallPlacement.h"
#include <optional>
#include <vector>

class CCBot;

// verifies undone wall placement, if it returns false
// then this is not a wall candidate (can't be neither a full wall, nor
struct WallCandidateVerifier {
    WallCandidateVerifier(
            const CCBot& bot,
            int baseLocationId,
            int startBaseLocationId,
            int enemyStartBaseLocationId
    );

    bool verifyPlacement(
            // {{lx, ly}, type} - position of a left-bottom most tile, and building size
            const std::vector<std::pair<std::pair<int,int>, BuildingType>>& alreadyPlaced,
            const std::vector<BuildingType>& buildingsLeft
    ) const;
private:
    const CCBot& m_bot;
    int m_baseLocationId;
    int m_startBaseLocationId;
    int m_enemyStartBaseLocationId;
};


