#include "WallManager.h"
#include <general/CCBot.h>

std::optional<CCPosition> WallManager::getBuildLocation(const UnitType &b) {
    if (!needWall || id >= 3) {
        // TODO: do not return empty if id > 3
        //  this will allow multiple-layer walling
        return {};
    }
    auto& lr = chosenPlacement.value().buildings[id].first;
    if (b.isSupplyProvider() && id == 0) {
        id++;
        return CCPosition(lr.first + 1, lr.second + 1);
    } else {
        if (abs(b.getFootPrintRadius() - 1.5) < 1e-9) {
            auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
            int pylons = m_bot.UnitInfo().getUnitTypeCount(Players::Self, pylonType);
            if (pylons == 0) {
                // cant build gate if pylons were not built
                return {};
            }
            id++;
            return CCPosition(lr.first + 1.5, lr.second + 1.5);
        }
    }
    return {};
}

WallManager::WallManager(CCBot& bot)
    : m_bot(bot)
{
}

void WallManager::onStart() {
    if (needWall) {
        srand(time(NULL));
        auto&& bases = m_bot.Bases();
        int myBaseId = (*bases.getOccupiedBaseLocations(Players::Self).begin())->getBaseId();
        auto wallPlacements = m_bot.MapMeta().getWallPlacements(myBaseId, myBaseId);
        BOT_ASSERT(wallPlacements.size() > 0, "No wall placements exist, but wall requested");
        while (true) {
            chosenPlacement = wallPlacements[rand() % wallPlacements.size()];
            if (chosenPlacement.value().wallType != WallType::FullWall) break;
        }
    }
}
