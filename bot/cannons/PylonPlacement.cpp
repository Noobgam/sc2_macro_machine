#include "PylonPlacement.h"
#include <algorithm>

bool PylonPlacement::operator==(const PylonPlacement &rhs) const {
    if (hash_code != rhs.hash_code) {
        return false;
    }
    return this->pylonPositions == rhs.pylonPositions;
}

PylonPlacement::PylonPlacement(std::vector<CCTilePosition> pylonPositions,
                               const std::map<CCPosition, CCPosition> &pylonReplacements) :
                               pylonReplacements(pylonReplacements) {
    std::sort(pylonPositions.begin(), pylonPositions.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.x != rhs.x) {
            return lhs.x < rhs.x;
        }
        return lhs.y < rhs.y;
    });
    this->pylonPositions = std::move(pylonPositions);
    size_t hash = 0;
    for (const auto& tile: this->pylonPositions) {
        hash *= 179;
        hash += tile.y;
        hash *= 179;
        hash += tile.x;
    }
    hash_code = hash;
}
