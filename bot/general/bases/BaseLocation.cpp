#include "BaseLocation.h"
#include "../../util/Util.h"
#include "../CCBot.h"
#include <iostream>
#include <util/LogInfo.h>

BaseLocation::BaseLocation(
        CCBot & bot,
        BaseLocationID baseID,
        BaseLocationProjection baseLocationProjection,
        const std::vector<const Resource*> & resources
)
    : m_bot(bot)
    , m_baseID(baseID)
    , m_left                 (std::numeric_limits<CCPositionType>::max())
    , m_right                (std::numeric_limits<CCPositionType>::lowest())
    , m_top                  (std::numeric_limits<CCPositionType>::lowest())
    , m_bottom               (std::numeric_limits<CCPositionType>::max())
{
    initialize(resources);
    m_depotActualPosition = baseLocationProjection.depotPos;
    finishInitialization();
}

BaseLocation::BaseLocation(CCBot & bot, BaseLocationID baseID, const std::vector<const Resource*> & resources)
    : m_bot(bot)
    , m_baseID               (baseID)
    , m_left                 (std::numeric_limits<CCPositionType>::max())
    , m_right                (std::numeric_limits<CCPositionType>::lowest())
    , m_top                  (std::numeric_limits<CCPositionType>::lowest())
    , m_bottom               (std::numeric_limits<CCPositionType>::max())
{
    initialize(resources);

    // calculate the depot position
    UnitType depot = Util::GetTownHall(m_bot.GetPlayerRequestedRace(Players::Self), m_bot);


    // the position of the depot will be the closest spot we can build one from the resource center

    bool foundDepot = false;
    for (auto & tile : m_distanceMap.getSortedTiles()) {
        // nexus is 5x5 so build position is the middle of the tile
        CCPosition buildPos(tile.x + .5, tile.y + .5);
        if (m_bot.Map().canBuildTypeAtPosition(buildPos.x, buildPos.y, depot)) {
            m_depotActualPosition = buildPos;
            foundDepot = true;
            break;
        }
    }
    BOT_ASSERT(foundDepot, "Could not find a place for nexus whatsoever.");

    auto startLoc = m_bot.Observation()->GetStartLocation();
    float dist1 = Util::Dist(m_centerOfResources, startLoc);
    float dist2 = Util::Dist(m_centerOfResources, m_depotActualPosition);
    if (dist1 < dist2) {
        // if this nexus is worse than our start location - start location is the actual center.
        m_depotActualPosition = startLoc;
    }
    finishInitialization();
}

const CCPosition & BaseLocation::getDepotActualPosition() const {
    return m_depotActualPosition;
}

bool BaseLocation::isInResourceBox(int tileX, int tileY) const {
    // technically this is not a box, rather a trapezoid. Probably doesnt matter whatsoever at this point.
    CCPositionType px = Util::TileToPosition((float)tileX);
    CCPositionType py = Util::TileToPosition((float)tileY);
    return px >= m_left && px < m_right && py < m_top && py >= m_bottom;
}

const std::vector<const Resource*> & BaseLocation::getGeysers() const {
    return m_geysers;
}

const std::vector<const Resource*> & BaseLocation::getMinerals() const {
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

void BaseLocation::draw() const {
    BRK_IF_NOT_DEBUG
    CCPositionType radius = Util::TileToPosition(1.0f);

    m_bot.Map().drawCircle(m_centerOfResources, radius, CCColor(255, 255, 0));

    std::stringstream ss;
    ss << "BaseLocation: " << m_baseID << "\n";
    ss << "Minerals:     " << m_minerals.size() << "\n";
    for (auto x : m_minerals) {
        ss << x->getID() << " ";
    }
    ss << "\n";
    ss << "Geysers:      " << m_geysers.size() << "\n";
    for (auto x : m_geysers) {
        ss << x->getID() << " ";
    }
    ss << "\n";
    ss << "Occupied By:  ";

//    if (isOccupiedByPlayer(Players::Self)) {
//        ss << "Self ";
//    }
//
//    if (isOccupiedByPlayer(Players::Enemy)) {
//        ss << "Enemy ";
//    }

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

    for (auto & mineral : m_minerals) {
        m_bot.Map().drawCircle(mineral->getPosition(), radius, CCColor(0, 128, 128));
        m_bot.Map().drawText(mineral->getPosition(), std::to_string(mineral->getID()));
    }

    for (auto & geyser : m_geysers) {
        m_bot.Map().drawCircle(geyser->getPosition(), radius, CCColor(0, 255, 0));
        m_bot.Map().drawText(geyser->getPosition(), std::to_string(geyser->getID()));
    }

    m_bot.Map().drawGroundCircle(m_depotActualPosition, radius, CCColor(255, 0, 0));

    m_bot.Map().drawTile(m_depotActualPosition.x, m_depotActualPosition.y, CCColor(0, 0, 255));

    //m_distanceMap.draw(m_bot);
}

bool BaseLocation::isMineralOnly() const {
    return getGeysers().empty();
}

void BaseLocation::resourceExpiredCallback(const Resource *resource) {
    LOG_DEBUG << "Removing resource from " << resource->getID() << " : " << this->getBaseId() << BOT_ENDL;
    if (resource->getResourceType() == ResourceType::MINERAL) {
        auto it = std::find(m_minerals.begin(), m_minerals.end(), resource);
        BOT_ASSERT(it != m_minerals.end(), "Trying to remove a mineral that I do not own");
        m_minerals.erase(it);
    } else {
        auto it = std::find(m_geysers.begin(), m_geysers.end(), resource);
        BOT_ASSERT(it != m_geysers.end(), "Trying to remove a geyser that I do not own");
        m_geysers.erase(it);
    }
}

BaseLocationID BaseLocation::getBaseId() const {
    return this->m_baseID;
}

void BaseLocation::initialize(const std::vector<const Resource*> & resources) {
    CCPositionType resourceCenterX = 0;
    CCPositionType resourceCenterY = 0;

    // add each of the resources to its corresponding container
    for (auto & resource : resources) {
        if (resource->getResourceType() == ResourceType::MINERAL) {
            m_minerals.push_back(resource);

            // add the position of the minerals to the center
            resourceCenterX += resource->getPosition().x;
            resourceCenterY += resource->getPosition().y;
        } else if (resource->getResourceType() == ResourceType::VESPENE) {
            m_geysers.push_back(resource);

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
}

void BaseLocation::finishInitialization() {
    m_left   = std::min(m_left,   m_depotActualPosition.x - 2.5f);
    m_right  = std::max(m_right,  m_depotActualPosition.x + 2.5f);
    m_top    = std::max(m_top,    m_depotActualPosition.y + 2.5f);
    m_bottom = std::min(m_bottom, m_depotActualPosition.y - 2.5f);
    m_distanceMap = m_bot.Map().getDistanceMap(m_depotActualPosition);
}

void BaseLocation::recalculateDistanceMap() {
    m_distanceMap = m_bot.Map().getDistanceMap(m_depotActualPosition);
}

const DistanceMap &BaseLocation::getDistanceMap() const {
    return m_distanceMap;
}

bool BaseLocation::containsResource(const Resource *resource) const {
    if (resource->getResourceType() == ResourceType::MINERAL) {
        return std::find(m_minerals.begin(), m_minerals.end(), resource) != m_minerals.end();
    } else if (resource->getResourceType() == ResourceType::VESPENE) {
        return std::find(m_geysers.begin(), m_geysers.end(), resource) != m_geysers.end();
    }
    BOT_ASSERT(false, "Unexpected resource type.");
    return false;
}
