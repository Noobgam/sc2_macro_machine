#include "DistanceMap.h"
#include "CCBot.h"

#include <general/map_meta/StaticMapMeta.h>
#include <util/Util.h>

const size_t LegalActions = 4;
const int actionX[LegalActions] = {1, -1, 0, 0};
const int actionY[LegalActions] = {0, 0, 1, -1};

DistanceMap::DistanceMap() 
{
    
}

int DistanceMap::getDistance(int tileX, int tileY) const
{ 
    BOT_ASSERT(tileX < m_width && tileY < m_height, "Index out of range: X = %d, Y = %d", tileX, tileY);
    return m_dist[tileX][tileY]; 
}

int DistanceMap::getDistance(const CCTilePosition & pos) const
{ 
    return getDistance(pos.x, pos.y); 
}

int DistanceMap::getDistance(const CCPosition & pos) const
{ 
    return getDistance(CCTilePosition((int)pos.x, (int)pos.y));
}

const std::vector<CCTilePosition> & DistanceMap::getSortedTiles() const
{
    return m_sortedTiles;
}

// Computes m_dist[x][y] = ground distance from (startX, startY) to (x,y)
// Uses BFS, since the map is quite large and DFS may cause a stack overflow
void DistanceMap::computeDistanceMap(CCBot & m_bot, const CCTilePosition & startTile)
{
    computeDistanceMap(
        [&m_bot](int x, int y) { return m_bot.Map().isWalkable(x, y); },
        startTile,
        m_bot.Map().getStaticMapMeta()
    );
}


// Computes m_dist[x][y] = ground distance from (startX, startY) to (x,y)
// Uses BFS, since the map is quite large and DFS may cause a stack overflow
void DistanceMap::computeDistanceMap(const StaticMapMeta & mapMeta, const CCTilePosition & startTile)
{
    computeDistanceMap([&mapMeta](int x, int y) { return mapMeta.isWalkable(x, y); }, startTile, mapMeta);
}

void DistanceMap::draw(CCBot & bot) const
{
    const int tilesToDraw = 200;
    for (size_t i(0); i < tilesToDraw; ++i)
    {
        auto & tile = m_sortedTiles[i];
        int dist = getDistance(tile);

        CCPosition textPos(tile.x + Util::TileToPosition(0.5), tile.y + Util::TileToPosition(0.5));
        std::stringstream ss;
        ss << dist;

        bot.Map().drawText(textPos, ss.str());
    }
}

const CCTilePosition & DistanceMap::getStartTile() const
{
    return m_startTile;
}

std::vector<CCTilePosition> DistanceMap::getPathTo(CCTilePosition pos) const {
    std::vector<CCTilePosition> res;
    res.reserve(m_dist[pos.x][pos.y] + 1);
    res.push_back(pos);
    auto is_valid = [this](const CCTilePosition& tile) {
        return tile.x >= 0 && tile.x < m_width && tile.y >= 0 && tile.y < m_height;
    };
    while (pos != m_startTile) {
        int curDist = m_dist[pos.x][pos.y];
        if (curDist == 0) {
            break;
        }
        std::optional<CCTilePosition> fromTile;
        for (size_t a=0; a<LegalActions; ++a) {
            CCTilePosition nextTile(pos.x - actionX[a], pos.y - actionY[a]);
            if (is_valid(nextTile) && getDistance(nextTile) == curDist - 1) {
                fromTile = nextTile;
                break;
            }
        }
        pos = fromTile.value();
        res.push_back(pos);
    }
    reverse(res.begin(), res.end());
    return res;
}

void DistanceMap::computeDistanceMap(
    std::function<bool(int,int)> walkable,
    const CCTilePosition startTile,
    const StaticMapMeta& mapMeta
) {
    m_startTile = startTile;
    m_width = mapMeta.width();
    m_height = mapMeta.height();
    m_dist = std::vector<std::vector<int>>(m_width, std::vector<int>(m_height, -1));
    m_sortedTiles.reserve(m_width * m_height);

    // the fringe for the BFS we will perform to calculate distances
    m_sortedTiles.push_back(startTile);

    m_dist[(int)startTile.x][(int)startTile.y] = 0;

    if (!walkable(startTile.x, startTile.y)) {
        // First move out of the wall. Allow distance map to move inside building it was asked to.
        for (size_t fringeIndex=0; fringeIndex < m_sortedTiles.size(); ++fringeIndex)
        {
            int x = m_sortedTiles[fringeIndex].x;
            int y = m_sortedTiles[fringeIndex].y;

            // check every possible child of this tile
            for (size_t a=0; a<LegalActions; ++a)
            {
                CCTilePosition nextTile(x + actionX[a], y + actionY[a]);


                // if the new tile is inside the map bounds,
                // is statically walkable
                // and is in a building now.
                if (mapMeta.isStaticallyWalkable(nextTile.x, nextTile.y)
                    && !walkable(nextTile.x, nextTile.y)
                    && getDistance(nextTile) == -1
                ) {
                    m_dist[(int)nextTile.x][(int)nextTile.y] = 0;
                    m_sortedTiles.push_back(nextTile);
                }
            }
        }
    }

    for (size_t fringeIndex=0; fringeIndex < m_sortedTiles.size(); ++fringeIndex)
    {
        int x = m_sortedTiles[fringeIndex].x;
        int y = m_sortedTiles[fringeIndex].y;
        int curDist = m_dist[x][y];

        // check every possible child of this tile
        for (size_t a=0; a<LegalActions; ++a)
        {
            CCTilePosition nextTile(x + actionX[a], y + actionY[a]);

            // if the new tile is inside the map bounds, is walkable, and has not been visited yet, set the distance of its parent + 1
            if (walkable(nextTile.x, nextTile.y) && getDistance(nextTile) == -1)
            {
                m_dist[(int)nextTile.x][(int)nextTile.y] = curDist + 1;
                m_sortedTiles.push_back(nextTile);
            }
        }
    }
}
