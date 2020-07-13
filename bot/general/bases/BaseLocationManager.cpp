#include "util/Util.h"
#include "BaseLocationManager.h"

#include "../CCBot.h"

BaseLocationManager::BaseLocationManager(CCBot & bot) : m_bot(bot) { }

void BaseLocationManager::onStart() {
    m_tileBaseLocations = std::vector<std::vector<BaseLocation *>>(m_bot.Map().width(), std::vector<BaseLocation *>(m_bot.Map().height(), nullptr));
    m_playerStartingBaseLocations[Players::Self]  = nullptr;
    m_playerStartingBaseLocations[Players::Enemy] = nullptr;

    // a BaseLocation will be anything where there are minerals to mine
    // so we will first look over all minerals and cluster them based on some distance
    const CCPositionType clusterDistance = Util::TileToPosition(12);

    // stores each cluster of resources based on some ground distance
    std::vector<std::vector<const Resource*>> resourceClusters;
    for (const auto& mineral : m_bot.getManagers().getResourceManager().getMinerals()) {
        bool foundCluster = false;
        for (auto & cluster : resourceClusters) {
            float dist = Util::Dist(mineral->getPosition(), Util::CalcCenter(cluster));

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
    for (const auto& geyser : m_bot.getManagers().getResourceManager().getGeysers()) {
        for (auto & cluster : resourceClusters) {
            //int groundDist = m_bot.Map().getGroundDistance(geyser.pos, Util::CalcCenter(cluster));
            float groundDist = Util::Dist(geyser->getPosition(), Util::CalcCenter(cluster));
            if (groundDist >= 0 && groundDist < clusterDistance) {
                cluster.push_back(geyser);
                break;
            }
        }
    }

    // add the base locations if there are more than 4 resouces in the cluster
    int baseID = 0;
    for (auto & cluster : resourceClusters) {
        if (cluster.size() > 4) {
            m_baseLocationData.emplace_back(m_bot, baseID++, cluster);
        }
    }

    // set start locations
    CCPosition selfStartLocation = m_bot.Observation()->GetStartLocation();
    auto& potentialLocations = m_bot.Observation()->GetGameInfo().enemy_start_locations;
    BOT_ASSERT(potentialLocations.size() == 1, "Multiple start locations are not supportd");
    CCPosition enemyStartLocation = potentialLocations[0];

    // construct the vectors of base location pointers, this is safe since they will never change
    for (auto & baseLocation : m_baseLocationData) {
        m_baseLocationPtrs.push_back(&baseLocation);
        if (baseLocation.containsPosition(selfStartLocation)) {
            baseLocation.setStartLocation(Players::Self);
            m_playerStartingBaseLocations[Players::Self] = &baseLocation;
        }
        if (baseLocation.containsPosition(enemyStartLocation)) {
            baseLocation.setStartLocation(Players::Enemy);
            m_playerStartingBaseLocations[Players::Enemy] = &baseLocation;
        }
    }
    BOT_ASSERT(m_playerStartingBaseLocations[Players::Self] != nullptr, "Self start location was not found");
    BOT_ASSERT(m_playerStartingBaseLocations[Players::Enemy] != nullptr, "Enemy start location was not found");

    // construct the map of tile positions to base locations
    for (int x = 0; x < m_bot.Map().width(); ++x) {
        for (int y=0; y < m_bot.Map().height(); ++y) {
            for (auto & baseLocation : m_baseLocationData) {
                CCPosition pos(Util::TileToPosition(x + 0.5f), Util::TileToPosition(y + 0.5f));
                if (baseLocation.containsPosition(pos)) {
                    m_tileBaseLocations[x][y] = &baseLocation;
                    break;
                }
            }
        }
    }

    // construct the sets of occupied base locations
    m_occupiedBaseLocations[Players::Self] = { m_playerStartingBaseLocations[Players::Self] };
    m_occupiedBaseLocations[Players::Enemy] = { m_playerStartingBaseLocations[Players::Enemy] };
}

void BaseLocationManager::onFrame() {
    drawBaseLocations();

    // reset the player occupation information for each location
    for (auto & baseLocation : m_baseLocationData) {
        baseLocation.setPlayerOccupying(Players::Self, false);
        baseLocation.setPlayerOccupying(Players::Enemy, false);
    }

    for (auto & player : {Players::Self, Players::Enemy}) {
        for (auto &unit : m_bot.UnitInfo().getUnits(player)) {
            // we only care about buildings on the ground
            if (!unit->getType().isBuilding() || unit->isFlying()) {
                continue;
            }

            BaseLocation *baseLocation = getBaseLocation(unit->getPosition());
            if (baseLocation != nullptr) {
                baseLocation->setPlayerOccupying(player, true);
            }
        }

        // set start location as occupied by enemy (TODO fix with time or smt)
        if (player == Players::Enemy) {
            auto position = m_playerStartingBaseLocations.find(Players::Enemy)->second->getPosition();
            getBaseLocation(position)->setPlayerOccupying(Players::Enemy, true);
        }
    }

    // update the occupied base locations for each player
    m_occupiedBaseLocations[Players::Self].clear();
    m_occupiedBaseLocations[Players::Enemy].clear();
    for (auto & baseLocation : m_baseLocationData) {
        if (baseLocation.isOccupiedByPlayer(Players::Self)) {
            m_occupiedBaseLocations[Players::Self].insert(&baseLocation);
        }
        if (baseLocation.isOccupiedByPlayer(Players::Enemy)) {
            m_occupiedBaseLocations[Players::Enemy].insert(&baseLocation);
        }
    }
}

BaseLocation * BaseLocationManager::getBaseLocation(const CCPosition & pos) const {
    if (!m_bot.Map().isValidPosition(pos)) { return nullptr; }
    return m_tileBaseLocations[(int)pos.x][(int)pos.y];
}

void BaseLocationManager::drawBaseLocations() {
    for (auto & baseLocation : m_baseLocationData) {
        baseLocation.draw();
    }

    // draw a purple sphere at the next expansion location
    CCPosition nextExpansionPosition = getNextExpansion(Players::Self);

    m_bot.Map().drawCircle(Util::GetPosition(nextExpansionPosition), 1, CCColor(255, 0, 255));
    m_bot.Map().drawText(Util::GetPosition(nextExpansionPosition), "Next Expansion Location", CCColor(255, 0, 255));
}

const std::vector<const BaseLocation *> & BaseLocationManager::getBaseLocations() const {
    return m_baseLocationPtrs;
}

const std::set<const BaseLocation *> & BaseLocationManager::getOccupiedBaseLocations(int player) const {
    return m_occupiedBaseLocations.at(player);
}


CCPosition BaseLocationManager::getNextExpansion(int player) const {
    const BaseLocation * homeBase =  m_playerStartingBaseLocations.at(player);
    const BaseLocation * closestBase = nullptr;
    int minDistance = std::numeric_limits<int>::max();

    for (auto & base : getBaseLocations()) {
        // skip mineral only and starting locations (TODO: fix this)
        if (base->isMineralOnly() || base->isOccupiedByPlayer(player)) {
            continue;
        }

        // get the tile position of the base
        auto tile = base->getDepotActualPosition();

        bool buildingInTheWay = false; // TODO: check if there are any units on the tile

        if (buildingInTheWay) {
            continue;
        }

        // the base's distance from our main nexus
        int distanceFromHome = homeBase->getGroundDistance(tile);

        // if it is not connected, continue
        if (distanceFromHome < 0)
        {
            continue;
        }

        if (!closestBase || distanceFromHome < minDistance)
        {
            closestBase = base;
            minDistance = distanceFromHome;
        }
    }

    return closestBase ? closestBase->getDepotActualPosition() : CCPosition(0, 0);
}

void BaseLocationManager::resourceExpiredCallback(const Resource* resource) {
    BaseLocation *baseLocation = getBaseLocation(resource->getPosition());
    baseLocation->resourceExpiredCallback(resource);
}
