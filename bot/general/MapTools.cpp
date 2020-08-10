#include "MapTools.h"
#include "../util/Util.h"
#include "CCBot.h"

#include <iostream>
#include <array>
#include <util/LogInfo.h>

namespace {
bool getBit(const sc2::ImageData& grid, int tileX, int tileY) {
    assert(grid.bits_per_pixel == 1);

    sc2::Point2DI pointI(tileX, tileY);
    if (pointI.x < 0 || pointI.x >= grid.width || pointI.y < 0 || pointI.y >= grid.height)
    {
        return false;
    }

    div_t idx = div(pointI.x + pointI.y * grid.width, 8);
    return (grid.data[idx.quot] >> (7 - idx.rem)) & 1;
}

}

const size_t LegalActions = 4;
const int actionX[LegalActions] ={1, -1, 0, 0};
const int actionY[LegalActions] ={0, 0, 1, -1};

typedef std::vector<std::vector<bool>> vvb;
typedef std::vector<std::vector<int>>  vvi;
typedef std::vector<std::vector<float>>  vvf;

#define HALF_TILE 0.5f

// constructor for MapTools
MapTools::MapTools(CCBot & bot)
    : m_bot     (bot)
    , m_maxZ    (0.0f)
    , m_frame   (0)
{
}

void MapTools::onStart()
{
    // can't load it earlier, map observations do not exist before onstart
    m_staticMapMeta = StaticMapMeta::getMeta(m_bot);

    auto m_width = m_staticMapMeta->width();
    auto m_height = m_staticMapMeta->height();
    m_lastSeen       = vvi(m_width, std::vector<int>(m_height, 0));
    m_powerMap       = vvi(m_width, std::vector<int>(m_height, 0));
    m_unbuildableNeutral = vvb(m_width, std::vector<bool>(m_height, false));
    m_unwalkableNeutral = vvb(m_width, std::vector<bool>(m_height, false));

    for (auto & unit : m_bot.Observation()->GetUnits())
    {
        m_maxZ = std::max(unit->pos.z, m_maxZ);
    }

    updateNeutralMap();
    // set tiles that static resources are on as unbuildable
}

void MapTools::onFrame()
{
    m_frame++;
    updatePowerMap();

    for (int x=0; x < m_staticMapMeta->width(); ++x)
    {
        for (int y=0; y < m_staticMapMeta->height(); ++y)
        {
            if (isVisible(x, y))
            {
                m_lastSeen[x][y] = m_frame;
            }
        }
    }

    draw();
}

bool MapTools::isExplored(const CCTilePosition & pos) const
{
    return isExplored(pos.x, pos.y);
}

bool MapTools::isExplored(const CCPosition & pos) const
{
    return isExplored(Util::GetTilePosition(pos));
}

bool MapTools::isExplored(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY)) { return false; }

    sc2::Visibility vis = m_bot.Observation()->GetVisibility(CCPosition(tileX + HALF_TILE, tileY + HALF_TILE));
    return vis == sc2::Visibility::Fogged || vis == sc2::Visibility::Visible;
}

bool MapTools::isVisible(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY)) { return false; }

    return m_bot.Observation()->GetVisibility(CCPosition(tileX + HALF_TILE, tileY + HALF_TILE)) == sc2::Visibility::Visible;
}

bool MapTools::isPowered(float x, float y) const
{
    return m_powerMap[(x * 2) + .5][(y * 2) + .5];
}

float MapTools::terrainHeight(float x, float y) const
{
    return m_staticMapMeta->getTerrainHeight(x, y);
}

int MapTools::getGroundDistance(const CCPosition & src, const CCPosition & dest) const
{
    if (m_allMaps.size() > 50)
    {
        m_allMaps.clear();
    }

    return getDistanceMap(src).getDistance(dest);
}

const DistanceMap & MapTools::getDistanceMap(const CCPosition & pos) const
{
    return getDistanceMap(Util::GetTilePosition(pos));
}

const DistanceMap & MapTools::getDistanceMap(const CCTilePosition & tile) const
{
    std::pair<int,int> pairTile(tile.x, tile.y);

    if (m_allMaps.find(pairTile) == m_allMaps.end())
    {
        m_allMaps[pairTile] = DistanceMap();
        m_allMaps[pairTile].computeDistanceMap(m_bot, tile);
    }

    return m_allMaps[pairTile];
}

int MapTools::getSectorNumber(int x, int y) const
{
    if (!isValidTile(x, y))
    {
        return 0;
    }

    return m_staticMapMeta->getSectorNumber(x, y);
}

bool MapTools::isValidTile(int tileX, int tileY) const
{
    return tileX >= 0 && tileY >= 0 && tileX < m_staticMapMeta->width() && tileY < m_staticMapMeta->height();
}

bool MapTools::isValidTile(const CCTilePosition & tile) const
{
    return isValidTile(tile.x, tile.y);
}

bool MapTools::isValidPosition(const CCPosition & pos) const
{
    return isValidTile(Util::GetTilePosition(pos));
}

void MapTools::drawLine(CCPositionType x1, CCPositionType y1, CCPositionType x2, CCPositionType y2, const CCColor & color) const
{
    m_bot.Debug()->DebugLineOut(sc2::Point3D(x1, y1, terrainHeight(x1, y1) + 0.05f), sc2::Point3D(x2, y2, terrainHeight(x2, y2) + 0.05f), color);
}

void MapTools::drawLine(const CCPosition & p1, const CCPosition & p2, const CCColor & color) const
{
    drawLine(p1.x, p1.y, p2.x, p2.y, color);
}

void MapTools::drawTile(int tileX, int tileY, const CCColor & color) const
{
    CCPositionType px = Util::TileToPosition((float)tileX) + Util::TileToPosition(0.1f);
    CCPositionType py = Util::TileToPosition((float)tileY) + Util::TileToPosition(0.1f);
    CCPositionType d  = Util::TileToPosition(0.8f);

    drawLine(px,     py,     px + d, py,     color);
    drawLine(px + d, py,     px + d, py + d, color);
    drawLine(px + d, py + d, px,     py + d, color);
    drawLine(px,     py + d, px,     py,     color);
}

void MapTools::drawHalfTile(float x, float y, const CCColor & color) const
{
    float d  = 0.05f;
    drawGroundCircle({x, y}, d, color);
}

void MapTools::drawBox(CCPositionType x1, CCPositionType y1, CCPositionType x2, CCPositionType y2, const CCColor & color) const
{
    m_bot.Debug()->DebugBoxOut(sc2::Point3D(x1, y1, m_maxZ + 2.0f), sc2::Point3D(x2, y2, m_maxZ-5.0f), color);
}

void MapTools::drawBox(const CCPosition & tl, const CCPosition & br, const CCColor & color) const
{
    m_bot.Debug()->DebugBoxOut(sc2::Point3D(tl.x, tl.y, m_maxZ + 2.0f), sc2::Point3D(br.x, br.y, m_maxZ-5.0f), color);
}

void MapTools::drawCircle(const CCPosition & pos, CCPositionType radius, const CCColor & color) const
{
    m_bot.Debug()->DebugSphereOut(sc2::Point3D(pos.x, pos.y, m_maxZ), radius, color);
}

void MapTools::drawGroundCircle(const CCPosition & pos, CCPositionType radius, const CCColor & color) const
{
    m_bot.Debug()->DebugSphereOut(sc2::Point3D(pos.x, pos.y, terrainHeight(pos)), radius, color);
}

void MapTools::drawCircle(CCPositionType x, CCPositionType y, CCPositionType radius, const CCColor & color) const
{
    m_bot.Debug()->DebugSphereOut(sc2::Point3D(x, y, m_maxZ), radius, color);
}


void MapTools::drawText(const CCPosition & pos, const std::string & str, const CCColor & color) const
{
    m_bot.Debug()->DebugTextOut(str, sc2::Point3D(pos.x, pos.y, m_maxZ), color);
}

void MapTools::drawTextScreen(float xPerc, float yPerc, const std::string & str, const CCColor & color) const
{
    m_bot.Debug()->DebugTextOut(str, CCPosition(xPerc, yPerc), color);
}

bool MapTools::isConnected(int x1, int y1, int x2, int y2) const
{
    if (!isValidTile(x1, y1) || !isValidTile(x2, y2))
    {
        return false;
    }

    int s1 = getSectorNumber(x1, y1);
    int s2 = getSectorNumber(x2, y2);

    return s1 != 0 && (s1 == s2);
}

bool MapTools::isConnected(const CCTilePosition & p1, const CCTilePosition & p2) const
{
    return isConnected(p1.x, p1.y, p2.x, p2.y);
}

bool MapTools::isConnected(const CCPosition & p1, const CCPosition & p2) const
{
    return isConnected(Util::GetTilePosition(p1), Util::GetTilePosition(p2));
}

bool MapTools::isBuildable(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY))
    {
        return false;
    }

    return m_staticMapMeta->isBuildable(tileX, tileY) && !m_unbuildableNeutral[tileX][tileY];
}

bool MapTools::canBuildTypeAtPosition(float tileX, float tileY, const UnitType & type) const
{
    return m_bot.Query()->Placement(m_bot.Data(type).buildAbility, CCPosition(tileX, tileY));
}

bool MapTools::isBuildable(const CCTilePosition & tile) const
{
    return isBuildable(tile.x, tile.y);
}

void MapTools::printMap()
{
    std::stringstream ss;
    for (int y(0); y < m_staticMapMeta->height(); ++y)
    {
        for (int x(0); x < m_staticMapMeta->width(); ++x)
        {
            ss << isWalkable(x, y);
        }

        ss << "\n";
    }

    std::ofstream out("map.txt");
    out << ss.str();
    out.close();
}

bool MapTools::isWalkable(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY))
    {
        return false;
    }

    return m_staticMapMeta->isWalkable(tileX, tileY) && !m_unwalkableNeutral[tileX][tileY];
}

bool MapTools::isWalkable(const CCTilePosition & tile) const
{
    return isWalkable(tile.x, tile.y);
}

int MapTools::width() const
{
    return m_staticMapMeta->width();
}

int MapTools::height() const
{
    return m_staticMapMeta->height();
}

const std::vector<CCTilePosition> & MapTools::getClosestTilesTo(const CCTilePosition & pos) const
{
    return getDistanceMap(pos).getSortedTiles();
}

bool MapTools::canWalk(int tileX, int tileY)
{
    return getBit(m_bot.Observation()->GetGameInfo().pathing_grid, tileX, tileY);
}

bool MapTools::canBuild(int tileX, int tileY) 
{
    return getBit(m_bot.Observation()->GetGameInfo().placement_grid, tileX, tileY);
}

float MapTools::terrainHeight(const CCPosition & point) const
{
    auto & grid = m_bot.Observation()->GetGameInfo().terrain_height;
    assert(grid.bits_per_pixel > 1);

    sc2::Point2DI pointI(static_cast<int>(point.x), static_cast<int>(point.y));
    if (pointI.x < 0 || pointI.x >= grid.width || pointI.y < 0 || pointI.y >= grid.height)
    {
        return 0.0f;
    }

    assert(grid.data.size() == static_cast<unsigned long>(grid.width * grid.height));
    unsigned char value = grid.data[pointI.x + pointI.y * grid.width];
    return (static_cast<float>(value) - 127.0f) / 8.f;
}


void MapTools::draw() const
{
}

bool MapTools::pylonPowers(const CCPosition& pylonPos, float radius, const CCPosition& candidate) const {
    float h1 = terrainHeight(pylonPos);
    float h2 = terrainHeight(candidate);
    if (h1 < h2) {
        return false;
    }
    return Util::Dist(pylonPos, candidate) < radius;
}

void MapTools::powerPylon(const CCPosition & pos, float r) {
    changePowering(pos, r, 1);
}

void MapTools::depowerPylon(const CCPosition & pos, float r) {
    changePowering(pos, r, -1);
}

void MapTools::updatePowerMap() {
    auto m_width = m_staticMapMeta->width();
    auto m_height = m_staticMapMeta->height();
    // TODO: remove this, use callbacks, depower dead pylons, power live pylons
    m_powerMap.assign(2 * m_width, std::vector<int>(2 * m_height, 0));
    for (auto & powerSource : m_bot.Observation()->GetPowerSources())
    {
        powerPylon(powerSource.position, powerSource.radius);
    }
}

void MapTools::changePowering(const CCPosition &pos, float radius, int d) {
    float x = pos.x;
    float y = pos.y;
    for (float i = (int)(x - radius); i <= x + radius; i += .5) {
        for (float j = (int)(y - radius); j <= y + radius; j += .5) {
            if (m_unbuildableNeutral[i][j]) continue;

            int idI = (2 * i + .5);
            int idJ = (2 * j + .5);

            if (pylonPowers(pos, radius, {i, j})) {
                m_powerMap[idI][idJ] += d;
            }
        }
    }
}

std::pair<int, int> MapTools::assumePylonBuilt(const CCPosition& pos, float radius) const {
    float x = pos.x;
    float y = pos.y;
    int freshlyPowered = 0;
    int poweredOnce    = 0;
    for (float i = (int)(x - radius); i <= x + radius; i += .5) {
        for (float j = (int)(y - radius); j <= y + radius; j += .5) {
            // this might be a little off if neutral is cleared.
            //  should not be as important, and will work if you update powering when neutrals died
            if (m_unbuildableNeutral[i][j]) continue;

            int idI = (2 * i + .5);
            int idJ = (2 * j + .5);

            if (pylonPowers(pos, radius, {i, j})) {
                int cnt = m_powerMap[idI][idJ];
                if (cnt == 0) {
                    freshlyPowered++;
                } else if (cnt == 1) {
                    poweredOnce++;
                }
            }
        }
    }
    return {freshlyPowered, poweredOnce};
}

void MapTools::updateNeutralMap() {
    auto&& neutrals = m_bot.UnitInfo().getUnits(Players::Neutral);
    for (auto & unitPtr : neutrals) {

        int width;
        int height;
        if (unitPtr->getType().isMineral() || unitPtr->getType().isGeyser()) {
            width = unitPtr->getType().tileWidth();
            height = unitPtr->getType().tileHeight();
        } else {
            // otherwise there's no data in tech tree to find from
            width =  unitPtr->getUnitPtr()->radius * 2;
            height = unitPtr->getUnitPtr()->radius * 2;
        }
        int tileX = std::floor(unitPtr->getPosition().x) - (width / 2);
        int tileY = std::floor(unitPtr->getPosition().y) - (height / 2);


        for (int x=tileX; x<tileX+width; ++x)
        {
            for (int y=tileY; y<tileY+height; ++y)
            {
                m_unbuildableNeutral[x][y] = m_unbuildableNeutral[x][y] || !Util::canBuildOnUnit (unitPtr->getType());
                m_unwalkableNeutral [x][y] = m_unwalkableNeutral[x][y]  || !Util::canWalkOverUnit(unitPtr->getType());
                if (!unitPtr->getType().isMineral() && !unitPtr->getType().isGeyser()) {
                    continue;
                }
            }
        }
    }
}

constexpr static int VISIBILITY_PROBES = 5;

bool MapTools::isVisible(const CCTilePosition &from, const CCTilePosition &to, float R) const {
    float fromHeight = terrainHeight(CCPosition(from.x + .5, from.y + .5));
    float toHeight = terrainHeight(CCPosition(to.x + .5, to.y + .5));
    // could there be a case where there is a highground between two points?
    if (toHeight > fromHeight) {
        return false;
    }
    int dx = from.x - to.x;
    int dy = from.y - to.y;
    if (dx * dx + dy * dy > R * R) {
        return false;
    }
    // if there is a highground in between, which is heigher than fromHeight - we won't be able to see it
    // iterative probing should be a good heuristic I guess
    float startx = from.x + .5;
    float starty = from.y + .5;
    float endx =   to.x + .5;
    float endy =   to.y + .5;
    for (int i = 1; i < VISIBILITY_PROBES - 1; ++i) {
        float curx = (startx * (VISIBILITY_PROBES - i) + endx * i) / VISIBILITY_PROBES;
        float cury = (starty * (VISIBILITY_PROBES - i) + endy * i) / VISIBILITY_PROBES;
        float height = terrainHeight(curx, cury);
        if (height > fromHeight) {
            return false;
        }
    }
    return true;
}

const StaticMapMeta& MapTools::getStaticMapMeta() const {
    return *m_staticMapMeta;
}

// this would allow us to avoid walling ourselves in. If we get pinned to the corner we'll still get stuck here.
CCPosition MapTools::findClosestWalkablePosition(const CCPosition &pos) const {
    if (isWalkable(pos.x, pos.y)) {
        return pos;
    }
    for (int r = 1; r < 3; ++r) {
        std::vector<std::pair<float, CCPosition>> positions;
        int y = pos.y - r;
        for (int x = pos.x - r; x <= pos.x + r; ++x) {
            if (isWalkable(x, y)) {
                CCPosition tileCenter = {x + .5f, y + .5f};
                float dist = Util::Dist(pos, tileCenter);
                positions.emplace_back(dist, tileCenter);
            }
        }
        if (!positions.empty()) {
            sort(positions.begin(), positions.end(), [](auto& lhs, auto& rhs) {
                return lhs.first < rhs.first;
            });
            return positions.begin()->second;
        }
    }

    // there's no point in searching for the position past r = 3
    return pos;
}
