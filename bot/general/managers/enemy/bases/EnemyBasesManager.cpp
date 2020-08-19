#include <general/managers/enemy/bases/EnemyBasesManager.h>
#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <util/Util.h>

#include "logging/Events.h"

EnemyBasesManager::EnemyBasesManager(CCBot &bot) : m_bot(bot) { }

void EnemyBasesManager::onStart() {
    auto& potentialLocations = m_bot.Observation()->GetGameInfo().enemy_start_locations;
    for (const auto& potentialLocation : potentialLocations) {
        const auto& baseLocation = m_bot.Bases().findBaseLocation(potentialLocation);
        if (baseLocation.has_value()) {
            expectAsOccupied(baseLocation.value());
        }
    }
}

void EnemyBasesManager::onFrame() {
    for (auto it = m_expectedBaseLocations.begin(); it != m_expectedBaseLocations.end();) {
        const auto& pos = Util::GetTilePosition((*it)->getPosition());
        bool isVisible = false;
        for (auto dx = -2; dx <= 2 && !isVisible; dx++) {
            for (auto dy = -2; dy <= 2 && !isVisible; dy++) {
                if (m_bot.Map().isVisible(pos.x + dx, pos.y + dy)) {
                    isVisible = true;
                }
            }
        }
        if (isVisible) {
            events::enemy_bases_manager::expectedBaseLocationIsNotOccupied(*it, true).log();
            it = m_expectedBaseLocations.erase(it);
        } else {
            it++;
        }
    }
    // TODO update occupied locations for flying depots
    draw();
}

void EnemyBasesManager::newUnitCallback(const Unit *unit) {
    if (unit->getPlayer() == Players::Enemy && unit->getType().isResourceDepot()) {
        LOG_DEBUG << "New depot found " << unit->getType().getName() << " " << unit->getID() << BOT_ENDL;
        m_depots.emplace_back(unit);
        std::optional<BaseLocation*> baseLocation = m_bot.Bases().findBaseLocation(unit->getPosition());
        events::enemy_bases_manager::resourceDepotWasDetected(unit, baseLocation).log();
        if (baseLocation.has_value()) {
            m_occupiedBaseLocations.insert({unit->getID(), baseLocation.value()});
            m_expectedBaseLocations.erase(baseLocation.value());
        }
    }
}

void EnemyBasesManager::unitDisappearedCallback(const Unit *unit) {
    if (unit->getPlayer() == Players::Enemy && unit->getType().isResourceDepot()) {
        m_depots.erase(std::remove(m_depots.begin(), m_depots.end(), unit), m_depots.end());
        std::optional<BaseLocation*> baseLocation = m_bot.Bases().findBaseLocation(unit->getPosition());
        events::enemy_bases_manager::resourceDepotIsDisappeared(unit, baseLocation).log();
        if (baseLocation.has_value()) {
            m_occupiedBaseLocations.erase(unit->getID());
        }
    }
}

void EnemyBasesManager::expectAsOccupied(const BaseLocation* baseLocation) {
    bool status = m_expectedBaseLocations.insert(baseLocation).second;
    events::enemy_bases_manager::expectBaseLocationAsOccupied(baseLocation, status).log();
}

void EnemyBasesManager::locationIsClear(const BaseLocation *baseLocation) {
    bool status = m_expectedBaseLocations.erase(baseLocation) != 0;
    events::enemy_bases_manager::expectedBaseLocationIsNotOccupied(baseLocation, status).log();
}

const std::vector<const Unit *>& EnemyBasesManager::getEnemyDepots() const {
    return m_depots;
}

const std::vector<const BaseLocation *> EnemyBasesManager::getExpectedEnemyBaseLocations() const {
    std::vector<const BaseLocation*> result;
    result.reserve(m_expectedBaseLocations.size());
    result.insert(result.end(), m_expectedBaseLocations.begin(), m_expectedBaseLocations.end());
    return result;
}

const std::vector<const BaseLocation *> EnemyBasesManager::getOccupiedEnemyBaseLocations() const {
    std::vector<const BaseLocation*> result;
    for (const auto& base : m_occupiedBaseLocations) {
        result.emplace_back(base.second);
    }
    return result;
}

const std::vector<const BaseLocation *> EnemyBasesManager::getAllExpectedEnemyBaseLocations() const {
    std::vector<const BaseLocation*> result;
    result.reserve(m_expectedBaseLocations.size());
    result.insert(result.end(), m_expectedBaseLocations.begin(), m_expectedBaseLocations.end());
    for (const auto& base : m_occupiedBaseLocations) {
        result.emplace_back(base.second);
    }
    return result;
}

void EnemyBasesManager::draw() {
#ifdef _DEBUG
    std::stringstream ss;
    ss << "Enemy has " << m_depots.size() << " known depots."<< "\n";
    if (!m_occupiedBaseLocations.empty()) {
        ss << "Occupied locations: ";
        for (const auto & baseLocation: m_occupiedBaseLocations) {
            ss << baseLocation.second->getBaseId() << " ";
        }
        ss << BOT_ENDL;
    }
    ss << "Expecting enemy on " << m_expectedBaseLocations.size() << " locations."<< "\n";
    if (!m_expectedBaseLocations.empty()) {
        ss << "Expected locations: ";
        for (const auto & baseLocation: m_expectedBaseLocations) {
            ss << baseLocation->getBaseId() << " ";
        }
        ss << BOT_ENDL;
    }
    m_bot.Map().drawTextScreen(0.85f, 0.6f, ss.str(), CCColor(255, 255, 0));
#endif
}
