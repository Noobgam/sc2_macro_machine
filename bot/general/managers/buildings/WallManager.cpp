#include "WallManager.h"
#include <general/CCBot.h>
#include <util/Util.h>
#include <util/LogInfo.h>

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
    , needWall(true)
{
}

void WallManager::onStart() {
    VALIDATE_CALLED_ONCE();
    if (needWall) {
        srand(time(NULL));
        auto&& bases = m_bot.getManagers().getBasesManager();
        int myBaseId = (*bases.getCompletedBases().begin())->getBaseLocation()->getBaseId();
        int baseId = m_bot.Map().getStaticMapMeta().getOrderedBasesByStartLocationId().at(myBaseId)[1];
        auto wallPlacements = m_bot.getMapMeta().getWallPlacements(myBaseId, baseId);
        if (wallPlacements.size() != 0) {
            while (true) {
                chosenPlacement = wallPlacements[rand() % wallPlacements.size()];
                if (chosenPlacement.value().wallType != WallType::FullWall) break;
            }
        } else {
            LOG_DEBUG << "No wall placements exist, but wall requested" << BOT_ENDL;

        }
    }
}

void WallManager::draw() {
    BRK_IF_NOT_DEBUG
    if (chosenPlacement.has_value()) {
        for (auto x : chosenPlacement->buildings) {
            int sz = 3;
            if (x.second == BuildingType::PoweringPylon) {
                sz = 2;
            }
            m_bot.Map().drawText({x.first.first + .0f, x.first.second + .0f}, "Wall part");
            for (int i = 0; i < sz; ++i) {
                for (int j = 0; j < sz; ++j) {
                    m_bot.Map().drawTile(i + x.first.first, j + x.first.second);
                }
            }
        }

        for (auto tile : chosenPlacement->gaps) {
            if (tile.second != GapType::OneByOne) continue;
            int x = tile.first.first;
            int y = tile.first.second;
            m_bot.Map().drawTile(x, y, CCColor(0, 255, 0));
        }
    } else {
    }
}
