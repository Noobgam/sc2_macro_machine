#include <util/LogInfo.h>
#include "ResourceManager.h"
#include "general/CCBot.h"

ResourceManager::ResourceManager(CCBot &bot): m_bot(bot) { }

const std::vector<const Mineral *> & ResourceManager::getMinerals() const {
    return m_mineralPtrs;
}

void ResourceManager::newUnitCallback(const Unit *unit) {
    const UnitType & type = unit->getType();
    if (type.isMineral()) {
        LOG_DEBUG << "New mineral" << endl;
        const auto & it = std::find_if(m_minerals.begin(), m_minerals.end(), [unit](auto & m) {
            return m->getPosition() == unit->getPosition();
        });
        if (it == m_minerals.end()) {
            const auto & mineralIt = m_minerals.emplace_back(std::make_unique<Mineral>(m_bot, unit, m_currentMineralID++));
            m_mineralPtrs.emplace_back(mineralIt.get());
        } else {
//             No idea why it happens
//            if (unit->getUnitPtr()->display_type == (*it)->getUnit()->getUnitPtr()->display_type) {
//                BOT_ASSERT(unit->getUnitPtr()->display_type == sc2::Unit::DisplayType::Snapshot, "Visible unit has changed");
//            }
            (*it)->updateUnit(unit);
        }
    }
}

void ResourceManager::unitDisappearedCallback(const Unit *unit) {
    const UnitType & type = unit->getType();
    if (type.isMineral()) {
        LOG_DEBUG << "Mineral disappeared" << endl;
        const auto & it = std::find_if(m_minerals.begin(), m_minerals.end(), [unit](auto & m) {
            return m->getPosition() == unit->getPosition();
        });
        BOT_ASSERT(it != m_minerals.end(), "Mineral was not found");
        const auto& mineral = *it;
        if (mineral->getLastUpdate() < m_bot.getObservationId()) {
            // mineral exhausted
            LOG_DEBUG << "Mineral field exhausted " << mineral->getPosition().x << ":" << mineral->getPosition().y << " id: " << mineral->getID() << endl;
            m_bot.Bases().mineralExpiredCallback(mineral.get());
            m_mineralPtrs.erase(std::find(m_mineralPtrs.begin(), m_mineralPtrs.end(), mineral.get()));
            m_minerals.erase(it);
        }
    }
}
