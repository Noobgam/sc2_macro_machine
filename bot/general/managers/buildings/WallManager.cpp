#include "WallManager.h"
#include <general/CCBot.h>
#include <util/Util.h>
#include <util/LogInfo.h>

std::optional<CCPosition> WallManager::getBuildLocation(const UnitType &b) {
    if (b.isRefinery()) {
        return {};
    }
    if (!needWall) {
        return {};
    }
    if (wallPartsAlive.size() == chosenPlacement.value().buildings.size()) {
        // TODO: do not return empty
        //  this will allow multiple-layer walling
        return {};
    }
    int size = b.tileWidth();
    int index = -1;
    for (auto& building : chosenPlacement.value().buildings) {
        ++index;
        int buildingSize = 2;
        if (building.second == BuildingType::ThreeByThree) {
            buildingSize = 3;
        }
        if (size != buildingSize) {
            continue;
        }
        bool taken = false;
        for (auto unit : wallPartsAlive) {
            if (findChosenBuildingId(unit) == index) {
                taken = true;
                break;
            }
        }
        if (taken) {
            continue;
        }
        CCPosition pos = {
                building.first.first + b.getFootPrintRadius(),
                building.first.second + b.getFootPrintRadius()
        };
        if (m_bot.Commander().getMacroManager().getBuildingPlacer().canBuildHere(
                pos.x,
                pos.y,
                b
        )) {
            return pos;
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

int WallManager::findChosenBuildingId(const Unit * unit) const {
    int x = unit->getPosition().x - unit->getType().getFootPrintRadius() + .5;
    int y = unit->getPosition().y - unit->getType().getFootPrintRadius() + .5;
    int sz = unit->getType().tileWidth();
    int index = -1;
    for (auto& building : chosenPlacement.value().buildings) {
        ++index;
        int buildingSize = 2;
        if (building.second == BuildingType::ThreeByThree) {
            buildingSize = 3;
        }
        if (building.first.first == x && building.first.second == y && sz == buildingSize) {
            return index;
        }
    }
    return -1;
}

void WallManager::newUnitCallback(const Unit * unit) {
    int index = findChosenBuildingId(unit);
    if (index != -1) {
        wallPartsAlive.push_back(unit);
    }
}

void WallManager::processRemoveUnit(const Unit *unit) {
    auto it = std::find(wallPartsAlive.begin(), wallPartsAlive.end(), unit);
    if (it != wallPartsAlive.end()) {
        wallPartsAlive.erase(it);
    }
}
