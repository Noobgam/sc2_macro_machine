#include "ScoutEnemyBaseOrder.h"
#include <general/CCBot.h>
#include <util/Util.h>

ScoutEnemyBaseOrder::ScoutEnemyBaseOrder(CCBot &bot, Squad *squad, const BaseLocation *baseLocation)
        : Order(bot, squad)
{
    m_scout = *m_squad->units().begin();
    scoutedLocation = baseLocation;
    currentDirection = scoutedLocation->getPosition();
}

void ScoutEnemyBaseOrder::onStart() {
    // first point doesnt matter as much
    chooseNewDirection();
}

void ScoutEnemyBaseOrder::onStep() {
    if (!m_scout.has_value()) {
        return;
    }
    // basic implementation idea:
    // (TODO) if in danger: focus on escaping from enemy units
    if (false) {

    } else {
        // else: wait until you can see the tile you're moving towards
        if (!m_bot.Map().isVisible(currentDirection.x, currentDirection.y)) {
            m_scout.value()->move(currentDirection);
            return;
        }
        // move towards closest tile that was not scouted recently within 15 distance from main location
        chooseNewDirection();
    }
}

void ScoutEnemyBaseOrder::onUnitAdded(const Unit *unit) {
    BOT_ASSERT(false, "Unit addition is not supported");
}

void ScoutEnemyBaseOrder::onUnitRemoved(const Unit *unit) {
    if (m_scout == unit) {
        m_scout = {};
        onEnd();
    }
}

void ScoutEnemyBaseOrder::chooseNewDirection() {
    auto res = findBestTile(200);
    if (!res.has_value()) {
        // we've recently scouted every spot in the base, lower the scouting threshold and try again
        res = findBestTile(50);
    }
    if (!res.has_value()) {
        // there's no target, consider running away from probes
        return;
    }
    currentDirection = Util::GetTileCenter(res.value());
}

std::optional<CCTilePosition> ScoutEnemyBaseOrder::findBestTile(int seenThreshold) {
    auto&& tiles = scoutedLocation->getDistanceMap().getSortedTiles();
    int currentFrame = m_bot.Map().getVisibilityFrame();
    std::optional<CCTilePosition> res;
    float minDist = 0;
    for (auto& tile : tiles) {
        int dst = scoutedLocation->getDistanceMap().getDistance(tile);
        if (dst < 5) {
            continue;
        }
        if (dst > 20) {
            break;
        }
        int lastSeen = m_bot.Map().getLastSeen(tile.x, tile.y);
        if (lastSeen != 0 && lastSeen >= currentFrame - seenThreshold) {
            continue;
        }
        float curDist = Util::Dist(m_scout.value(), Util::GetTileCenter(tile));
        if (!res.has_value() || minDist > curDist) {
            res = tile;
            minDist = curDist;
        }
    }
    return res;
}


