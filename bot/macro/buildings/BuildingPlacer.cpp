#include "../../general/model/Common.h"
#include "../../general/CCBot.h"
#include "../../util/Util.h"
#include "BuildingPlacer.h"
#include "../../util/LogInfo.h"

BuildingPlacer::BuildingPlacer(CCBot & bot)
    : m_bot(bot)
{

}

void BuildingPlacer::onStart()
{
    m_reserveMap = std::vector< std::vector<bool> >(m_bot.Map().width(), std::vector<bool>(m_bot.Map().height(), false));
}

bool BuildingPlacer::isInResourceBox(int tileX, int tileY) const
{
    for (auto & base : m_bot.Bases().getOccupiedBaseLocations(Players::Self)) {
        if (base->isInResourceBox(tileX, tileY)) {
            return true;
        }
    }
    return false;
}

// makes final checks to see if a building can be built at a certain location
bool BuildingPlacer::canBuildHere(float x, float y, const UnitType & type) const
{
    if (isInResourceBox(x, y)) {
        return false;
    }

    // check the reserve map
    int lx = x - type.getFootPrintRadius() + .5;
    int ly = y - type.getFootPrintRadius() + .5;
    for (int cx = lx; cx < lx + type.tileWidth(); cx++) {
        for (int cy = ly; cy < ly + type.tileHeight(); cy++) {

            if (!m_bot.Map().isValidTile(cx, cy) || m_reserveMap[cx][cy]) {
                return false;
            }
            if (!m_bot.Map().isBuildable(cx, cy)) {
                return false;
            }
        }
    }
    return buildable(type, x, y);

}

static inline double heuristic(int newlyPowered, [[maybe_unused]] int doublePowered, double distFromBase) {
    // as dist increases, power should decrease.
    // when distance is no longer required - double powering should come into place
    return newlyPowered - 5 * distFromBase;
}

std::optional<CCPosition> BuildingPlacer::getBuildLocation(const UnitType & b) const {
    if (b.isRefinery()) {
        return getRefineryPosition();
    }
    static int id = 0;
    static auto& wallPlacement = m_bot.chosenPlacement;
    if (m_bot.NeedWall() && id < 3 && wallPlacement.has_value()) {
        auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
        auto& lr = wallPlacement.value().buildings[id].first;
        if (b.isSupplyProvider()) {
            return CCPosition(lr.first + 1, lr.second + 1);
        } else {
            int pylons = m_bot.UnitInfo().getUnitTypeCount(Players::Self, pylonType);
            if (pylons == 0) {
                // cant build gate if pylons were not built
                return {};
            }
            ++id;
            return CCPosition(lr.first + 1.5, lr.second + 1.5);
        }
    } else {
        double bestHeuristic = std::numeric_limits<double>::min();
        std::optional<CCPosition> bestPosO;
        auto& myBases = m_bot.Bases().getOccupiedBaseLocations(Players::Self);
        BOT_ASSERT(!myBases.empty(), "No bases found, no idea where to build");
        auto& firstBase = *myBases.begin();
        if (b.isSupplyProvider()) {
            // perhaps moving this logic to some sort of 'PylonPlacer' would be better
            // copy is intended here
            auto closestToStart = m_bot.Map().getClosestTilesTo(firstBase->getDepotActualPosition());
            // pylons are built in corners of tiles.
            for (size_t i(0); i < closestToStart.size() && i < 1000; ++i) {
                const auto & pos = closestToStart[i];
                if (canBuildHere(pos.x, pos.y, b)) {
                    auto&& lr = m_bot.Map().assumePylonBuilt(Util::GetPosition(pos), 6.5f);
                    int dist = m_bot.Map().getGroundDistance(firstBase->getDepotActualPosition(), Util::GetPosition(pos));
                    double curHeuristic = heuristic(
                            lr.first,
                            lr.second,
                            dist
                            );
                    if (curHeuristic > bestHeuristic) {
                        bestPosO = Util::GetPosition(pos);
                        bestHeuristic = curHeuristic;
                    }
                    // if there aint no good pylons or there is a very good one it doesnt really matter
                    if (bestHeuristic > 400) break;
                }
            }
            return bestPosO;
        } else {
            auto closestToStart = m_bot.Map().getClosestTilesTo(firstBase->getDepotActualPosition());

            bool isRound = Util::isRound(b.getFootPrintRadius());
            for (size_t i(0); i < closestToStart.size() && i < 1000; ++i) {
                auto & pos = closestToStart[i];
                CCPosition cand = isRound
                        ? CCPosition(pos.x, pos.y)
                        : CCPosition(pos.x + .5, pos.y + .5);
                if (canBuildHere(cand.x, cand.y, b)) {
                    return cand;
                }
            }
        }
    }

    return {};
}

bool BuildingPlacer::tileOverlapsBaseLocation(int x, int y, UnitType type) const
{
    // if it's a resource depot we don't care if it overlaps
    if (type.isResourceDepot())
    {
        return false;
    }

    // dimensions of the proposed location
    int tx1 = x;
    int ty1 = y;
    int tx2 = tx1 + type.tileWidth();
    int ty2 = ty1 + type.tileHeight();

    // for each base location
    for (const BaseLocation * base : m_bot.Bases().getBaseLocations())
    {
        // dimensions of the base location
        int bx1 = (int)base->getDepotActualPosition().x;
        int by1 = (int)base->getDepotActualPosition().y;
        auto&& xx = Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot);
        int bx2 = bx1 + xx.tileWidth();
        int by2 = by1 + xx.tileHeight();

        // conditions for non-overlap are easy
        bool noOverlap = (tx2 < bx1) || (tx1 > bx2) || (ty2 < by1) || (ty1 > by2);

        // if the reverse is true, return true
        if (!noOverlap)
        {
            return true;
        }
    }

    // otherwise there is no overlap
    return false;
}

bool BuildingPlacer::buildable(const UnitType & type, float x, float y) const
{
    // TODO: does this take units on the map into account?
    if (!m_bot.Map().isValidTile(x, y) || !m_bot.Map().canBuildTypeAtPosition(x, y, type))
    {
        return false;
    }

    // todo: check that it won't block an addon

    return true;
}

void BuildingPlacer::reserveTiles(int bx, int by, int width, int height)
{
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();
    for (int x = bx; x < bx + width && x < rwidth; x++)
    {
        for (int y = by; y < by + height && y < rheight; y++)
        {
            m_reserveMap[x][y] = true;
        }
    }
}

void BuildingPlacer::drawReservedTiles()
{
#ifdef _DEBUG
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();

    for (int x = 0; x < rwidth; ++x)
    {
        for (int y = 0; y < rheight; ++y)
        {
            if (m_reserveMap[x][y] || isInResourceBox(x, y))
            {
                m_bot.Map().drawTile(x, y, CCColor(255, 255, 0));
            }
        }
    }
//    for (int x = 0; x < 2 * rwidth; ++x) {
//        for (int y = 0; y < 2 * rheight; ++y) {
//            if (x % 2 != y % 2) continue;
//            if (m_bot.Map().isPowered(x * .5, y * .5)) {
//                m_bot.Map().drawHalfTile(x * .5, y * .5, CCColor(0, 200, 0));
//            }
//        }
//    }
#endif
}

void BuildingPlacer::freeTiles(int bx, int by, int width, int height)
{
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();

    for (int x = bx; x < bx + width && x < rwidth; x++)
    {
        for (int y = by; y < by + height && y < rheight; y++)
        {
            m_reserveMap[x][y] = false;
        }
    }
}

std::optional<CCPosition> BuildingPlacer::getRefineryPosition() const {
    CCPosition closestGeyser(0, 0);
    double minGeyserDistanceFromHome = std::numeric_limits<double>::max();
    CCPosition homePosition = m_bot.GetStartLocation();

    UnitType refinery = Util::GetRefinery(m_bot.GetPlayerRace(Players::Self), m_bot);

    for (auto & unitPtr : m_bot.UnitInfo().getUnits(Players::Neutral)) {
        auto& unit = *unitPtr;
        // unit must be a geyser
        if (!unit.getType().isGeyser())
        {
            continue;
        }

        CCPosition geyserPos(unit.getPosition());
        
        // can't build a refinery on top of another
        if (!m_bot.Map().canBuildTypeAtPosition(geyserPos.x, geyserPos.y, refinery))
        {
            continue;
        }

        // check to see if it's next to one of our depots
        bool nearDepot = false;
        for (auto & unitPtr : m_bot.UnitInfo().getUnits(Players::Self)) {
            const Unit& unit = *unitPtr;
            if (unit.getType().isResourceDepot() && Util::Dist(unit, geyserPos) < 10)
            {
                nearDepot = true;
                break;
            }
        }

        if (nearDepot)
        {
            double homeDistance = Util::Dist(unit, homePosition);

            if (homeDistance < minGeyserDistanceFromHome)
            {
                minGeyserDistanceFromHome = homeDistance;
                closestGeyser = unit.getPosition();
            }
        }
    }

    return CCPosition (closestGeyser.x, closestGeyser.y);
}

bool BuildingPlacer::isReserved(int x, int y) const
{
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();
    if (x < 0 || y < 0 || x >= rwidth || y >= rheight)
    {
        return false;
    }

    return m_reserveMap[x][y];
}

