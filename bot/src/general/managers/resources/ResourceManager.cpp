#include <util/LogInfo.h>
#include "ResourceManager.h"
#include "general/CCBot.h"

ResourceManager::ResourceManager(CCBot &bot): m_bot(bot) { }

void ResourceManager::onStart() {
    for (const auto& unit : m_bot.UnitInfo().getUnits(Players::Neutral)) {
        newUnitCallback(unit);
    }
}

const std::vector<const Resource *> & ResourceManager::getMinerals() const {
    return m_mineralPtrs;
}

const std::vector<const Resource *> &ResourceManager::getGeysers() const {
    return m_geysersPtrs;
}

void ResourceManager::newUnitCallback(const Unit *unit) {
    const UnitType & type = unit->getType();
    if (type.isMineral() || type.isGeyser()) {
        auto& resources = type.isMineral() ? m_minerals : m_geysers;
        auto& resourcesPtrs = type.isMineral() ? m_mineralPtrs : m_geysersPtrs;
        const auto & it = std::find_if(resources.begin(), resources.end(), [unit](auto & r) {
            return r->getPosition() == unit->getPosition();
        });
        if (it == resources.end()) {
            const auto & resourceIt = resources.emplace_back(std::make_unique<Resource>(m_bot, unit, m_currentResourceID++));
            resourcesPtrs.emplace_back(resourceIt.get());
        } else {
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
            m_bot.Bases().resourceExpiredCallback(mineral.get());
            m_mineralPtrs.erase(std::find(m_mineralPtrs.begin(), m_mineralPtrs.end(), mineral.get()));
            m_minerals.erase(it);
        }
    } else if (type.isGeyser()) {
        // do nothing, geyser is always there
    }
}
