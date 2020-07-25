#include "StaticMapMeta.h"

#include <general/CCBot.h>
#include <string>
#include <util/Util.h>
#include <util/LogInfo.h>
#include <util/BotAssert.h>
#include <set>

using std::vector;
using std::string;

#define all(x) x.begin(), x.end()

namespace {
    bool getBit(const sc2::ImageData &grid, int tileX, int tileY) {
        assert(grid.bits_per_pixel == 1);

        sc2::Point2DI pointI(tileX, tileY);
        if (pointI.x < 0 || pointI.x >= grid.width || pointI.y < 0 || pointI.y >= grid.height) {
            return false;
        }

        div_t idx = div(pointI.x + pointI.y * grid.width, 8);
        return (grid.data[idx.quot] >> (7 - idx.rem)) & 1;
    }

    float terrainHeight(const sc2::ImageData &grid, int x, int y) {
        assert(grid.bits_per_pixel > 1);

        sc2::Point2DI pointI(x, y);
        if (pointI.x < 0 || pointI.x >= grid.width || pointI.y < 0 || pointI.y >= grid.height) {
            return 0.0f;
        }

        assert(grid.data.size() == static_cast<unsigned long>(grid.width * grid.height));
        unsigned char value = grid.data[pointI.x + pointI.y * grid.width];
        return (static_cast<float>(value) - 127.0f) / 8.f;
    }

    bool isGeyser(sc2::UnitTypeID m_type)
    {
        switch (m_type.ToType())
        {
            case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER         : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER  : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER   : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER     : return true;
            default: return false;
        }
    }

    bool isMineral(sc2::UnitTypeID m_type)
    {
        switch (m_type.ToType())
        {
            case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD            : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750         : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD        : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750     : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD		   : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750	   : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750 : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD    : return true;
            case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD: return true;
            case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750: return true;
            case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD450         : return true;
            default: return false;
        }
    }
}

StaticMapMeta::StaticMapMeta() {}

StaticMapMeta::StaticMapMeta(const CCBot &bot) {
    m_width = bot.Observation()->GetGameInfo().width;
    m_height = bot.Observation()->GetGameInfo().height;
    m_walkable = vector<vector<bool>>(m_width, std::vector<bool>(m_height, true));
    m_buildable = vector<vector<bool>>(m_width, std::vector<bool>(m_height, false));
    m_sectorNumber = vector<vector<int>>(m_width, std::vector<int>(m_height, 0));
    m_terrainHeight = vector<vector<float>>(m_width, std::vector<float>(m_height, 0.0f));
    m_unbuildableNeutral = vector<vector<bool>>(m_width, std::vector<bool>(m_height, false));
    m_unwalkableNeutral = vector<vector<bool>>(m_width, std::vector<bool>(m_height, false));

    auto &&canWalk = [&bot](int tileX, int tileY) {
        return getBit(bot.Observation()->GetGameInfo().pathing_grid, tileX, tileY);
    };

    auto &&canBuild = [&bot](int tileX, int tileY) {
        return getBit(bot.Observation()->GetGameInfo().placement_grid, tileX, tileY);
    };

    for (int x(0); x < m_width; ++x) {
        for (int y(0); y < m_height; ++y) {
            m_buildable[x][y] = canBuild(x, y);
            m_walkable[x][y] = m_buildable[x][y] || canWalk(x, y);
            m_terrainHeight[x][y] = terrainHeight(bot.Observation()->GetGameInfo().terrain_height, x, y);
        }
    }

    // set tiles that static resources are on as unbuildable
    auto &&neutrals = bot.Observation()->GetUnits(sc2::Unit::Alliance::Neutral);
    for (auto &unitPtr : neutrals) {

        int width;
        int height;
        UnitType unitType = UnitType(
                unitPtr->unit_type,
                const_cast<CCBot&>(bot)
        );
        if (unitType.isMineral() || unitType.isGeyser()) {
            width = unitType.tileWidth();
            height = unitType.tileHeight();
        } else {
            // otherwise there's no data in tech tree to find from
            width = unitPtr->radius * 2;
            height = unitPtr->radius * 2;
        }
        int tileX = std::floor(unitPtr->pos.x) - (width / 2);
        int tileY = std::floor(unitPtr->pos.y) - (height / 2);


        for (int x = tileX; x < tileX + width; ++x) {
            for (int y = tileY; y < tileY + height; ++y) {
                m_unbuildableNeutral[x][y] = m_unbuildableNeutral[x][y] || !Util::canBuildOnUnit(unitType);
                m_unwalkableNeutral[x][y] = m_unwalkableNeutral[x][y] || !Util::canWalkOverUnit(unitType);
                if (!unitType.isMineral() && !unitType.isGeyser()) {
                    continue;
                }
            }
        }
    }

    computeConnectivity();
    m_baseLocationProjections = calculateBaseLocations(bot);
    if (bot.Observation()->GetGameInfo().map_name.rfind("Test", 0) == 0) {
        // implied convention on test maps you can start on any location
        for (auto& x : m_baseLocationProjections) {
            m_startLocationIds.push_back(x.baseId);
        }
    } else {
        auto locations = bot.Observation()->GetGameInfo().start_locations;

        for (auto& loc : locations) {
            auto it = std::find_if(all(m_baseLocationProjections), [&loc](auto& base) {
                return Util::Dist(loc, base.depotPos) < 1;
            });
            if (it != m_baseLocationProjections.end()) {
                m_startLocationIds.push_back(it->baseId);
            }
        }
    }
}

std::unique_ptr<StaticMapMeta> StaticMapMeta::getMeta(const CCBot &bot) {
    string mapName = bot.Observation()->GetGameInfo().map_name;
    string fileName = "data/static_map_metas/" + mapName;
    if (FileUtils::fileExists(fileName)) {
        std::unique_ptr<StaticMapMeta> meta;
        std::ifstream ifs = FileUtils::openReadFile(fileName);
        boost::archive::text_iarchive ia(ifs);
        ia >> meta;
        LOG_DEBUG << "Successfully loaded map [" + mapName + "] from stash" << endl;
        return meta;
    } else {
        LOG_DEBUG << "Could not find a map [" + mapName + "] in stash, will recalculate" << endl;
        auto ptr = std::make_unique<StaticMapMeta>(bot);
        auto ofs = FileUtils::openWriteFile(fileName);
        boost::archive::text_oarchive oa(ofs);
        oa << ptr;
        return ptr;
    }
}

std::unique_ptr<StaticMapMeta> StaticMapMeta::getMeta(string mapName) {
    string fileName = "data/static_map_metas/" + mapName;
    if (!FileUtils::fileExists(fileName)) {
        LOG_DEBUG << "Static meta for map [" << fileName << "] was not calculated" << endl;
        std::terminate();
    }
    std::unique_ptr<StaticMapMeta> meta;
    std::ifstream ifs = FileUtils::openReadFile(fileName);
    boost::archive::text_iarchive ia(ifs);
    ia >> meta;
    return meta;
}

void StaticMapMeta::computeConnectivity() {
    const static int LEGAL_ACTIONS = 8;
    const static int actionX[LEGAL_ACTIONS] = {-1, -1, -1, 0, 1, 1, 1, 0};
    const static int actionY[LEGAL_ACTIONS] = {-1, 0, 1, 1, 1, 0, -1, -1};

    std::vector<std::array<int, 2>> fringe;
    fringe.reserve(m_width * m_height);
    int sectorNumber = 0;

    // for every tile on the map, do a connected flood fill using BFS
    for (int x = 0; x < m_width; ++x) {
        for (int y = 0; y < m_height; ++y) {
            // if the sector is not currently 0, or the map isn't walkable here, then we can skip this tile
            if (m_sectorNumber[x][y] != 0 || !isWalkable(x, y)) {
                continue;
            }

            // increase the sector number, so that walkable tiles have sectors 1-N
            sectorNumber++;

            // reset the fringe for the search and add the start tile to it
            fringe.clear();
            fringe.push_back({x, y});
            m_sectorNumber[x][y] = sectorNumber;

            // do the BFS, stopping when we reach the last element of the fringe
            for (size_t fringeIndex = 0; fringeIndex < fringe.size(); ++fringeIndex) {
                auto &tile = fringe[fringeIndex];

                // check every possible child of this tile
                for (size_t a = 0; a < LEGAL_ACTIONS; ++a) {
                    int nextX = tile[0] + actionX[a];
                    int nextY = tile[1] + actionY[a];

                    // if the new tile is inside the map bounds, is walkable, and has not been assigned a sector, add it to the current sector and the fringe
                    if (isValidTile(nextX, nextY) && isWalkable(nextX, nextY) && m_sectorNumber[nextX][nextY] == 0) {
                        m_sectorNumber[nextX][nextY] = sectorNumber;
                        fringe.push_back({nextX, nextY});
                    }
                }
            }
        }
    }
}

bool StaticMapMeta::isValidTile(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

bool StaticMapMeta::isValidTile(CCTilePosition tile) const {
    return isValidTile(tile.x, tile.y);
}

bool StaticMapMeta::isBuildable(int tileX, int tileY) const {
    if (!isValidTile(tileX, tileY)) {
        return false;
    }

    return m_buildable[tileX][tileY] && !m_unbuildableNeutral[tileX][tileY];
}

bool StaticMapMeta::pylonPowers(const CCPosition& pylonPos, float radius, const CCPosition& candidate) const {
    float h1 = m_terrainHeight[pylonPos.x][pylonPos.y];
    float h2 = m_terrainHeight[candidate.x][candidate.y];
    if (h1 < h2) {
        return false;
    }
    return Util::Dist(pylonPos, candidate) < radius;
}

bool StaticMapMeta::isWalkable(int tileX, int tileY) const {
    if (!isValidTile(tileX, tileY)) {
        return false;
    }
    return m_walkable[tileX][tileY] && !m_unwalkableNeutral[tileX][tileY];
}

int StaticMapMeta::width() const {
    return m_width;
}

int StaticMapMeta::height() const {
    return m_height;
}

const std::vector<BaseLocationProjection> &StaticMapMeta::getBaseLocations() const {
    return m_baseLocationProjections;
}

const std::vector<int>& StaticMapMeta::getStartLocationIds() const {
    return m_startLocationIds;
}

int StaticMapMeta::getSectorNumber(int tileX, int tileY) const {
    return m_sectorNumber[tileX][tileY];
}

float StaticMapMeta::getTerrainHeight(float x, float y) const {
    return m_terrainHeight[x][y];
}

DistanceMap StaticMapMeta::getDistanceMap(const CCPosition& pos) const {
    DistanceMap mp{};
    mp.computeDistanceMap(*this, CCTilePosition(pos.x, pos.y));
    return mp;
}

DistanceMap StaticMapMeta::getDistanceMap(const CCTilePosition & pos) const {
    DistanceMap mp{};
    mp.computeDistanceMap(*this, CCTilePosition(pos.x, pos.y));
    return mp;
}

std::vector<std::vector<const sc2::Unit *>> StaticMapMeta::findResourceClusters(const CCBot& bot) {
    // a BaseLocation will be anything where there are minerals to mine
    // so we will first look over all minerals and cluster them based on some distance
    const CCPositionType clusterDistance = Util::TileToPosition(12);

    // stores each cluster of resources based on some ground distance
    std::vector<std::vector<const sc2::Unit*>> resourceClusters;
    std::function<bool(const sc2::Unit& unit)> mineralFilter = [](const sc2::Unit& unit) {
        return isMineral(unit.unit_type);
    };
    std::function<bool(const sc2::Unit& unit)> geyserFilter = [](const sc2::Unit& unit) {
        return isGeyser(unit.unit_type);
    };

    for (auto & mineral : bot.Observation()->GetUnits(mineralFilter)) {
        // skip minerals that don't have more than 100 starting minerals
        // these are probably stupid map-blocking minerals to confuse us
        bool foundCluster = false;
        for (auto & cluster : resourceClusters) {
            float dist = Util::Dist(mineral->pos, Util::CalcCenter(cluster));

            // quick initial air distance check to eliminate most resources
            if (dist < clusterDistance) {
                // now do a more expensive ground distance check
                float groundDist = dist; //m_bot.Map().getGroundDistance(mineral.pos, Util::CalcCenter(cluster));
                if (groundDist >= 0 && groundDist < clusterDistance) {
                    cluster.push_back(mineral);
                    foundCluster = true;
                    break;
                }
            }
        }

        if (!foundCluster) {
            resourceClusters.emplace_back();
            resourceClusters.back().push_back(mineral);
        }
    }

    // add geysers only to existing resource clusters
    for (auto & geyser : bot.Observation()->GetUnits(geyserFilter)) {
        for (auto & cluster : resourceClusters) {
            //int groundDist = m_bot.Map().getGroundDistance(geyser.pos, Util::CalcCenter(cluster));
            float groundDist = Util::Dist(geyser->pos, Util::CalcCenter(cluster));
            if (groundDist >= 0 && groundDist < clusterDistance) {
                cluster.push_back(geyser);
                break;
            }
        }
    }
    return resourceClusters;
}

std::vector<BaseLocationProjection> StaticMapMeta::calculateBaseLocations(const CCBot& bot) {
    auto&& resourceClusters = findResourceClusters(bot);
    int baseId = 0;
    sort(resourceClusters.begin(), resourceClusters.end(), [](const auto& lhs, const auto& rhs) {
        auto&& lpos = Util::CalcCenter(lhs);
        auto&& rpos = Util::CalcCenter(rhs);
        if (lpos.x != rpos.x) return lpos.x < rpos.x;
        return lpos.y < rpos.y;
    });
    std::vector<BaseLocationProjection> projections;
    projections.reserve(resourceClusters.size());
    CCPosition pos = bot.GetStartLocation();
    const int N = 10;
    for (auto& cluster : resourceClusters) {
        CCPosition center = Util::CalcCenter(cluster);
        vector <CCPosition> closest;
        for (int i = -N; i <= N; ++i) {
            for (int j = -N; j <= N; ++j) {
                closest.push_back({center.x + i, center.y + j});
            }
        }
        sort(closest.begin(), closest.end(), [center](CCPosition l, CCPosition r) {
            double lx = (l.x - center.x);
            double ly = (l.y - center.y);

            double rx = (r.x - center.x);
            double ry = (r.y - center.y);

            return (lx * lx + ly * ly) < (rx * rx + ry * ry);
        });
        UnitType depot = Util::GetTownHall(bot.GetPlayerRace(Players::Self), const_cast<CCBot&>(bot));
        if (Util::Dist(pos, center) < 2 * N) {
            projections.push_back({baseId++, center, pos});
            continue;
        }
        for (auto tile : closest) {
            int x = tile.x;
            int y = tile.y;
            if (bot.Map().canBuildTypeAtPosition(x + .5, y + .5, depot)) {
                projections.push_back({baseId++, center, CCPosition(x + .5, y + .5)});
                break;
            }
        }
    }
    return projections;
}

constexpr static int VISIBILITY_PROBES = 9;

// statically this function is more precise than runtime alternative
bool StaticMapMeta::isVisible(const CCTilePosition &from, const CCTilePosition &to, float R) const {
    float fromHeight = m_terrainHeight[from.x][from.y];
    float toHeight =   m_terrainHeight[to.x][to.y];
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
        float height = m_terrainHeight[curx][cury];
        if (height > fromHeight) {
            return false;
        }
    }
    return true;
}

#undef all