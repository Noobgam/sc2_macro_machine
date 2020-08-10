#include "util/Util.h"
#include "BaseLocationManager.h"

#include "../CCBot.h"

BaseLocationManager::BaseLocationManager(CCBot & bot) : m_bot(bot) { }

void BaseLocationManager::onStart() {
    VALIDATE_CALLED_ONCE();

    m_tileBaseLocations = std::vector<std::vector<BaseLocation *>>(m_bot.Map().width(), std::vector<BaseLocation *>(m_bot.Map().height(), nullptr));
    m_playerStartingBaseLocations[Players::Self]  = nullptr;
    m_playerStartingBaseLocations[Players::Enemy] = nullptr;

    std::vector<std::vector<const Resource*>> resourceClusters = findResourceClusters();
    // add the base locations if there are more than 4 resouces in the cluster
    BaseLocationID baseID = 0;
    sort(resourceClusters.begin(), resourceClusters.end(), [](const auto& lhs, const auto& rhs) {
       auto&& lpos = Util::CalcCenter(lhs);
       auto&& rpos = Util::CalcCenter(rhs);
       if (lpos.x != rpos.x) return lpos.x < rpos.x;
       return lpos.y < rpos.y;
    });
    for (auto & cluster : resourceClusters) {
        if (cluster.size() > 4) {
            auto id = baseID++;
            auto&& projection = m_bot.Map().getStaticMapMeta().getBaseLocations()[id];
            m_baseLocationData.insert({id, std::make_unique<BaseLocation>(
                    m_bot,
                    id,
                    projection,
                    cluster
            )});
        }
    }

    CCPosition selfStartLocation = m_bot.Observation()->GetStartLocation();
    for (auto &locationPair : m_baseLocationData) {
        const auto &baseLocation = locationPair.second.get();
        if (baseLocation->containsPosition(selfStartLocation)) {
            baseLocation->setStartLocation(Players::Self);
            baseLocation->setPlayerHasDepot(Players::Self, true);
            m_playerStartingBaseLocations[Players::Self] = baseLocation;
            break;
        }
    }
    auto& potentialLocations = m_bot.Observation()->GetGameInfo().enemy_start_locations;

    if (m_bot.Observation()->GetGameInfo().map_name.rfind("Test", 0) == 0) {
        // cruth for test maps, so they could be played 1x0.
        for (auto& locationPair : m_baseLocationData) {
            const auto &baseLocation = locationPair.second.get();
            if (!baseLocation->isPlayerStartLocation(Players::Self)) {
                baseLocation->setStartLocation(Players::Enemy);
                m_playerStartingBaseLocations[Players::Enemy] = baseLocation;
                break;
            }
        }
    } else {
        BOT_ASSERT(potentialLocations.size() == 1, "Multiple start locations are not supportd");
        CCPosition enemyStartLocation = potentialLocations[0];
        // construct the vectors of base location pointers, this is safe since they will never change
        for (auto &locationPair : m_baseLocationData) {
            const auto &baseLocation = locationPair.second.get();
            if (baseLocation->containsPosition(enemyStartLocation)) {
                baseLocation->setStartLocation(Players::Enemy);
                baseLocation->setPlayerHasDepot(Players::Enemy, true);
                m_playerStartingBaseLocations[Players::Enemy] = baseLocation;
                break;
            }
        }
    }

    for (auto& locationPair : m_baseLocationData) {
        const auto &baseLocation = locationPair.second.get();
        m_baseLocationPtrs.push_back(baseLocation);
    }

    BOT_ASSERT(m_playerStartingBaseLocations[Players::Self] != nullptr, "Self start location was not found");
    BOT_ASSERT(m_playerStartingBaseLocations[Players::Enemy] != nullptr, "Enemy start location was not found");

    // construct the map of tile positions to base locations
    for (int x = 0; x < m_bot.Map().width(); ++x) {
        for (int y = 0; y < m_bot.Map().height(); ++y) {
            CCPosition pos(Util::TileToPosition(x + 0.5f), Util::TileToPosition(y + 0.5f));
            std::vector<std::pair<int, BaseLocation*>> distBaseLocationPair;
            distBaseLocationPair.reserve(m_baseLocationData.size());
            for (auto & locationPair : m_baseLocationData) {
                const auto & baseLocation = locationPair.second.get();
                distBaseLocationPair.emplace_back(
                        baseLocation->getGroundDistance(pos),
                        baseLocation
                );
            }
            sort(distBaseLocationPair.begin(), distBaseLocationPair.end());
            m_tileBaseLocations[x][y] = distBaseLocationPair[0].second;
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
        baseLocation.second->setPlayerOccupying(Players::Self, false);
        baseLocation.second->setPlayerOccupying(Players::Enemy, false);
        baseLocation.second->setPlayerHasDepot(Players::Self, false);
        baseLocation.second->setPlayerHasDepot(Players::Enemy, false);
    }

    for (auto & player : {Players::Self, Players::Enemy}) {
        for (auto &unit : m_bot.UnitInfo().getUnits(player)) {
            // we only care about buildings on the ground
            if (!unit->getType().isBuilding() || unit->isFlying()) {
                continue;
            }

            BaseLocation *baseLocation = getBaseLocation(unit->getPosition());
            if (baseLocation != nullptr) {
                if (unit->getType().isResourceDepot() && unit->isCompleted()) {
                    baseLocation->setPlayerHasDepot(player, true);
                }
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
    for (const auto & baseLocation : m_baseLocationPtrs) {
        if (baseLocation->isOccupiedByPlayer(Players::Self)) {
            m_occupiedBaseLocations[Players::Self].insert(baseLocation);
        }
        if (baseLocation->isOccupiedByPlayer(Players::Enemy)) {
            m_occupiedBaseLocations[Players::Enemy].insert(baseLocation);
        }
    }
}

BaseLocation * BaseLocationManager::getBaseLocation(const Resource* resource) const {
    for (auto baseLocation : m_baseLocationPtrs) {
        if (baseLocation->containsResource(resource)) {
            return baseLocation;
        }
    }
    BOT_ASSERT(false, "Could not find a base for this resource");
    return nullptr;
}

BaseLocation * BaseLocationManager::getBaseLocation(const CCPosition & pos) const {
    if (!m_bot.Map().isValidPosition(pos)) { return nullptr; }
    return m_tileBaseLocations[(int)pos.x][(int)pos.y];
}

std::vector<std::vector<const Resource *>> BaseLocationManager::findResourceClusters() const {
    // a BaseLocation will be anything where there are minerals to mine
    // so we will first look over all minerals and cluster them based on some distance
    const CCPositionType clusterDistance = Util::TileToPosition(12);

    // stores each cluster of resources based on some ground distance
    std::vector<std::vector<const Resource*>> resourceClusters;
    for (auto & mineral : m_bot.getManagers().getResourceManager().getMinerals()) {
        // skip minerals that don't have more than 100 starting minerals
        // these are probably stupid map-blocking minerals to confuse us
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
    for (auto & geyser : m_bot.getManagers().getResourceManager().getGeysers()) {
        for (auto & cluster : resourceClusters) {
            //int groundDist = m_bot.Map().getGroundDistance(geyser.pos, Util::CalcCenter(cluster));
            float groundDist = Util::Dist(geyser->getPosition(), Util::CalcCenter(cluster));
            if (groundDist >= 0 && groundDist < clusterDistance) {
                cluster.push_back(geyser);
                break;
            }
        }
    }
    return resourceClusters;
}

void BaseLocationManager::drawBaseLocations() {
    for (const auto & baseLocation : m_baseLocationPtrs) {
        baseLocation->draw();
    }
    for (const auto& loc : m_bot.Map().getStaticMapMeta().getBaseLocations()) {
        m_bot.Map().drawCircle(loc.depotPos, 2, CCColor(0, 0, 255));
    }

    // draw a purple sphere at the next expansion location
    CCPosition nextExpansionPosition = getNextExpansion(Players::Self);

    m_bot.Map().drawCircle(Util::GetPosition(nextExpansionPosition), 1, CCColor(255, 0, 255));
    m_bot.Map().drawText(Util::GetPosition(nextExpansionPosition), "Next Expansion Location", CCColor(255, 0, 255));
}

const std::vector<BaseLocation *> & BaseLocationManager::getBaseLocations() const {
    return m_baseLocationPtrs;
}

const BaseLocation *BaseLocationManager::getBaseLocation(BaseLocationID id) const {
    const auto & it = m_baseLocationData.find(id);
    BOT_ASSERT(it != m_baseLocationData.end(), "Base location was not found");
    return it->second.get();
}

const std::set<const BaseLocation *> & BaseLocationManager::getOccupiedBaseLocations(int player) const {
    return m_occupiedBaseLocations.at(player);
}

CCPosition BaseLocationManager::getNextExpansion(int player) const {
    const BaseLocation * homeBase =  m_playerStartingBaseLocations.at(player);
    const BaseLocation * closestBase = nullptr;
    int minDistance = std::numeric_limits<int>::max();

    const auto& bases = m_bot.getManagers().getBasesManager().getBases();
    for (auto & baseLocation : getBaseLocations()) {
        const auto baseIt = std::find_if(bases.begin(), bases.end(), [baseLocation](auto b) {
            return baseLocation == b->getBaseLocation();
        });

        // skip mineral only and starting locations (TODO: fix this)
        if (baseLocation->isMineralOnly() || baseIt != bases.end()) {
            continue;
        }

        // get the tile position of the base
        auto tile = baseLocation->getDepotActualPosition();

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
            closestBase = baseLocation;
            minDistance = distanceFromHome;
        }
    }

    return closestBase ? closestBase->getDepotActualPosition() : CCPosition(0, 0);
}

void BaseLocationManager::resourceExpiredCallback(const Resource* resource) {
    BaseLocation *baseLocation = getBaseLocation(resource);
    baseLocation->resourceExpiredCallback(resource);
}

const BaseLocation * BaseLocationManager::getPlayerStartLocation(CCPlayer player) const {
    return m_playerStartingBaseLocations.at(player);
}
