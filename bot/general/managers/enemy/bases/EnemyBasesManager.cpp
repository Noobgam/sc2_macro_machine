#include <general/managers/enemy/bases/EnemyBasesManager.h>
#include <general/CCBot.h>
#include <util/LogInfo.h>

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
    // TODO update occupied locations for flying depots
    draw();
}

void EnemyBasesManager::newUnitCallback(const Unit *unit) {
    if (unit->getPlayer() == Players::Enemy && unit->getType().isResourceDepot()) {
        LOG_DEBUG << "New depot found " << unit->getType().getName() << " " << unit->getID() << endl;
        m_depots.emplace_back(unit);
        std::optional<BaseLocation*> baseLocation = m_bot.Bases().findBaseLocation(unit->getPosition());
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
        if (baseLocation.has_value()) {
            m_occupiedBaseLocations.erase(unit->getID());
        }
    }
}

void EnemyBasesManager::expectAsOccupied(const BaseLocation* baseLocation) {
    m_expectedBaseLocations.insert(baseLocation);
}

void EnemyBasesManager::locationIsClear(const BaseLocation *baseLocation) {
    m_expectedBaseLocations.erase(baseLocation);
}

const std::vector<const Unit *>& EnemyBasesManager::getEnemyDepots() const {
    return m_depots;
}

const std::set<const BaseLocation *> EnemyBasesManager::getExpectedEnemyBaseLocations() const {
    std::set<const BaseLocation*> result;
    result.insert(m_expectedBaseLocations.begin(), m_expectedBaseLocations.end());
    for (const auto& base : m_occupiedBaseLocations) {
        result.insert(base.second);
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
        ss << endl;
    }
    ss << "Expecting enemy on " << m_expectedBaseLocations.size() << " locations."<< "\n";
    if (!m_expectedBaseLocations.empty()) {
        ss << "Expected locations: ";
        for (const auto & baseLocation: m_expectedBaseLocations) {
            ss << baseLocation->getBaseId() << " ";
        }
        ss << endl;
    }
    m_bot.Map().drawTextScreen(0.85f, 0.6f, ss.str(), CCColor(255, 255, 0));
#endif
}
