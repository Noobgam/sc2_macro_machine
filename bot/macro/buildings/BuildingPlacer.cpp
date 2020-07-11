#include "../../general/model/Common.h"
#include "../../general/CCBot.h"
#include "../../util/Util.h"
#include "BuildingPlacer.h"
#include "Building.h"

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
bool BuildingPlacer::canBuildHere(int bx, int by, const Building & b) const
{
    if (isInResourceBox(bx, by))
    {
        return false;
    }

    // check the reserve map
    for (int x = bx; x < bx + b.type.tileWidth(); x++)
    {
        for (int y = by; y < by + b.type.tileHeight(); y++)
        {
            if (!m_bot.Map().isValidTile(x, y) || m_reserveMap[x][y])
            {
                return false;
            }
        }
    }

    // if it overlaps a base location return false
    if (tileOverlapsBaseLocation(bx, by, b.type))
    {
        return false;
    }

    return true;
}

//returns true if we can build this type of unit here with the specified amount of space.
bool BuildingPlacer::canBuildHereWithSpace(int bx, int by, const Building & b, int buildDist) const
{
    UnitType type = b.type;

    //if we can't build here, we of course can't build here with space
    if (!canBuildHere(bx, by, b))
    {
        return false;
    }

    // height and width of the building
    int width  = type.tileWidth();
    int height = type.tileHeight();

    // TODO: make sure we leave space for add-ons. These types of units can have addons:

    // define the rectangle of the building spot
    int startx = bx - buildDist;
    int starty = by - buildDist;
    int endx   = bx + width + buildDist - 1;
    int endy   = by + height + buildDist - 1;

    // TODO: recalculate start and end positions for addons

    // if this rectangle doesn't fit on the map we can't build here
    if (startx < 0 || starty < 0 || endx > m_bot.Map().width() || endx < bx + width - 1 || endy > m_bot.Map().height() || endy < by + height - 1)
    {
        return false;
    }

    // if we can't build here, or space is reserved, or it's in the resource box, we can't build here
    for (int x = startx; x < endx; x++)
    {
        for (int y = starty; y < endy; y++)
        {
            if (!type.isRefinery())
            {
                if (!buildable(b, x, y) || m_reserveMap[x][y])
                {
                    return false;
                }
            }
        }
    }

    return true;
}

CCPosition BuildingPlacer::getBuildLocationNear(const Building & b, int buildDist) const
{
    Timer t;
    t.start();

    // get the precomputed vector of tile positions which are sorted closes to this location
    auto & closestToBuilding = m_bot.Map().getClosestTilesTo(b.desiredPosition);

    double ms1 = t.getElapsedTimeInMilliSec();

    // iterate through the list until we've found a suitable location
    for (size_t i(0); i < closestToBuilding.size() && i < 1000; ++i)
    {
        auto & pos = closestToBuilding[i];

        if (canBuildHereWithSpace(pos.x, pos.y, b, buildDist))
        {
            double ms = t.getElapsedTimeInMilliSec();
            //printf("Building Placer Took %d iterations, lasting %lf ms @ %lf iterations/ms, %lf setup ms\n", (int)i, ms, (i / ms), ms1);

            return Util::GetPosition(pos);
        }
    }

    double ms = t.getElapsedTimeInMilliSec();
    //printf("Building Placer Failure: %s - Took %lf ms\n", b.type.getName().c_str(), ms);

    return CCPosition(0, 0);
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

bool BuildingPlacer::buildable(const Building & b, int x, int y) const
{
    // TODO: does this take units on the map into account?
    if (!m_bot.Map().isValidTile(x, y) || !m_bot.Map().canBuildTypeAtPosition(x, y, b.type))
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
    for (int x = 0; x < 2 * rwidth; ++x) {
        for (int y = 0; y < 2 * rheight; ++y) {
            if (x % 2 != y % 2) continue;
            if (m_bot.Map().isPowered(x * .5, y * .5)) {
                m_bot.Map().drawHalfTile(x * .5, y * .5, CCColor(0, 200, 0));
            }
        }
    }
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

CCPosition BuildingPlacer::getRefineryPosition()
{
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
        if (!m_bot.Map().canBuildTypeAtPosition((int)geyserPos.x, (int)geyserPos.y, refinery))
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

