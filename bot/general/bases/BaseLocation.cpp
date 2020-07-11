#include "BaseLocation.h"
#include "../../util/Util.h"
#include "../CCBot.h"
#include <sstream>
#include <iostream>

const int NearBaseLocationTileDistance = 20;

BaseLocation::BaseLocation(CCBot & bot, int baseID, const std::vector<const Unit*> & resources)
    : m_bot(bot)
    , m_baseID               (baseID)
    , m_isStartLocation      (false)
    , m_left                 (std::numeric_limits<CCPositionType>::max())
    , m_right                (std::numeric_limits<CCPositionType>::lowest())
    , m_top                  (std::numeric_limits<CCPositionType>::lowest())
    , m_bottom               (std::numeric_limits<CCPositionType>::max())
{
    m_isPlayerStartLocation[Players::Self] = false;
    m_isPlayerStartLocation[Players::Enemy] = false;
    m_isPlayerOccupying[Players::Self] = false;
    m_isPlayerOccupying[Players::Enemy] = false;

    CCPositionType resourceCenterX = 0;
    CCPositionType resourceCenterY = 0;

    // add each of the resources to its corresponding container
     for (auto & resource : resources) {
        if (resource->getType().isMineral()) {
            m_minerals.push_back(resource);
            m_mineralPositions.push_back(resource->getPosition());

            // add the position of the minerals to the center
            resourceCenterX += resource->getPosition().x;
            resourceCenterY += resource->getPosition().y;
        } else if (resource->getType().isGeyser()) {
            m_geysers.push_back(resource);
            m_geyserPositions.push_back(resource->getPosition());

            // pull the resource center toward the geyser if it exists
            resourceCenterX += resource->getPosition().x;
            resourceCenterY += resource->getPosition().y;
        } else {
            // if there is existing nexus - we probably need to recalc something
        }

        // set the limits of the base location bounding box
        CCPositionType resWidth = Util::TileToPosition(1);
        CCPositionType resHeight = Util::TileToPosition(0.5);

        m_left   = std::min(m_left,   resource->getPosition().x - resWidth);
        m_right  = std::max(m_right,  resource->getPosition().x + resWidth);
        m_top    = std::max(m_top,    resource->getPosition().y + resHeight);
        m_bottom = std::min(m_bottom, resource->getPosition().y - resHeight);
    }
     m_centerOfResources = CCPosition(m_left + (m_right - m_left) / 2, m_top + (m_bottom - m_top) / 2);

    // compute this BaseLocation's DistanceMap, which will compute the ground distance
    // from the center of its resources to every other tile on the map
    m_distanceMap = m_bot.Map().getDistanceMap(m_centerOfResources);

    // calculate the depot position
    UnitType depot = Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot);

    // this doesnt work for starting location, probably we need to pass existing nexus into this method

    // the position of the depot will be the closest spot we can build one from the resource center
    for (auto & tile : m_distanceMap.getSortedTiles()) {
        // nexus is 5x5 so build position is the middle of the tile
        CCPosition buildPos(tile.x + .5, tile.y + .5);
        if (m_bot.Map().canBuildTypeAtPosition(buildPos.x, buildPos.y, depot)) {
            m_depotActualPosition = buildPos;
            break;
        }
    }

    m_left   = std::min(m_left,   m_depotActualPosition.x - 2.5f);
    m_right  = std::max(m_right,  m_depotActualPosition.x + 2.5f);
    m_top    = std::max(m_top,    m_depotActualPosition.y + 2.5f);
    m_bottom = std::min(m_bottom, m_depotActualPosition.y - 2.5f);
}

const CCPosition & BaseLocation::getDepotActualPosition() const {
    return m_depotActualPosition;
}

void BaseLocation::setStartLocation(CCPlayer player) {
    m_isPlayerOccupying[player] = true;
    m_isPlayerStartLocation[player] = true;
    m_isStartLocation = true;
}

void BaseLocation::setPlayerOccupying(CCPlayer player, bool occupying) {
    m_isPlayerOccupying[player] = occupying;
}

bool BaseLocation::isInResourceBox(int tileX, int tileY) const {
    // technically this is not a box, rather a trapezoid. Probably doesnt matter whatsoever at this point.
    CCPositionType px = Util::TileToPosition((float)tileX);
    CCPositionType py = Util::TileToPosition((float)tileY);
    return px >= m_left && px < m_right && py < m_top && py >= m_bottom;
}

bool BaseLocation::isOccupiedByPlayer(CCPlayer player) const {
    return m_isPlayerOccupying.at(player);
}

bool BaseLocation::isExplored() const {
    return m_bot.Map().isExplored(m_centerOfResources);
}

bool BaseLocation::isPlayerStartLocation(CCPlayer player) const {
    return m_isPlayerStartLocation.at(player);
}

bool BaseLocation::containsPosition(const CCPosition & pos) const {
    if (!m_bot.Map().isValidPosition(pos) || (pos.x == 0 && pos.y == 0)) {
        return false;
    }
    return getGroundDistance(pos) < NearBaseLocationTileDistance;
}

const std::vector<const Unit*> & BaseLocation::getGeysers() const {
    return m_geysers;
}

const std::vector<const Unit*> & BaseLocation::getMinerals() const {
    return m_minerals;
}

const CCPosition & BaseLocation::getPosition() const {
    return m_centerOfResources;
}

int BaseLocation::getGroundDistance(const CCPosition & pos) const {
    return m_distanceMap.getDistance(pos);
}

int BaseLocation::getGroundDistance(const CCTilePosition & pos) const {
    return m_distanceMap.getDistance(pos);
}

void BaseLocation::draw() {
#ifdef _DEBUG
    CCPositionType radius = Util::TileToPosition(1.0f);

    m_bot.Map().drawCircle(m_centerOfResources, radius, CCColor(255, 255, 0));

    std::stringstream ss;
    ss << "BaseLocation: " << m_baseID << "\n";
    ss << "Start Loc:    " << (m_isStartLocation ? "true" : "false") << "\n";
    ss << "Minerals:     " << m_mineralPositions.size() << "\n";
    ss << "Geysers:      " << m_geyserPositions.size() << "\n";
    ss << "Occupied By:  ";

    if (isOccupiedByPlayer(Players::Self)) {
        ss << "Self ";
    }

    if (isOccupiedByPlayer(Players::Enemy)) {
        ss << "Enemy ";
    }

    m_bot.Map().drawText(CCPosition(m_left, m_top+3), ss.str().c_str());
    m_bot.Map().drawText(CCPosition(m_left, m_bottom), ss.str().c_str());

    // draw the base bounding box
    m_bot.Map().drawBox(m_left, m_top, m_right, m_bottom);

    for (CCPositionType x=m_left; x < m_right; x += Util::TileToPosition(1.0f)) {
        //m_bot.Map().drawLine(x, m_top, x, m_bottom, CCColor(160, 160, 160));
    }

    for (CCPositionType y=m_bottom; y<m_top; y += Util::TileToPosition(1.0f)) {
        //m_bot.Map().drawLine(m_left, y, m_right, y, CCColor(160, 160, 160));
    }

    for (auto & mineralPos : m_mineralPositions) {
        m_bot.Map().drawCircle(mineralPos, radius, CCColor(0, 128, 128));
    }

    for (auto & geyserPos : m_geyserPositions) {
        m_bot.Map().drawCircle(geyserPos, radius, CCColor(0, 255, 0));
    }

    m_bot.Map().drawGroundCircle(m_depotActualPosition, radius, CCColor(255, 0, 0));

    m_bot.Map().drawTile(m_depotActualPosition.x, m_depotActualPosition.y, CCColor(0, 0, 255));

    //m_distanceMap.draw(m_bot);
#endif
}

bool BaseLocation::isMineralOnly() const {
    return getGeysers().empty();
}
