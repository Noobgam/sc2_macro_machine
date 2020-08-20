#include <util/LogInfo.h>
#include "Base.h"
#include "general/bases/BaseLocation.h"

Base::Base(CCBot &bot, const BaseLocation *baseLocation, const Unit *nexus):
    m_bot(bot),
    m_baseLocation(baseLocation),
    m_nexus(nexus)
{
    m_workers = std::make_unique<BaseWorkers>(bot, this);
}

const BaseLocation *Base::getBaseLocation() const {
    return m_baseLocation;
}

BaseWorkers *Base::getBaseWorkers() const {
    return m_workers.get();
}

const Unit *Base::getNexus() const {
    return m_nexus;
}

std::vector<std::pair<const Unit *, const Resource *>> Base::getAssimilators() const {
    return m_assimilators;
}

std::vector<std::pair<const Unit *, const Resource *>> Base:: getActiveAssimilators() const {
    std::vector<std::pair<const Unit *, const Resource *>> activeAssimilators;
    for (const auto& assimilator : m_assimilators) {
        if (assimilator.first->isCompleted() && assimilator.second->getResourceAmount() != 0) {
            activeAssimilators.emplace_back(assimilator);
        }
    }
    return activeAssimilators;
}

void Base::onNewAssimilator(const Unit *assimilator, const Resource *geyser) {
    LOG_DEBUG << "[BASES_MANAGER] New assimilator " << assimilator->getID() << " was added to base on location " << this->getBaseLocation()->getBaseId() << ". Geyser: " << geyser->getID() << BOT_ENDL;
    m_assimilators.emplace_back(assimilator, geyser);
    BOT_ASSERT(m_assimilators.size() <= this->getBaseLocation()->getGeysers().size(), "More assimilators then geysers.");
}

void Base::onAssimilatorDestroyed(const Unit *assimilator) {
    int initialLength = m_assimilators.size();
    m_assimilators.erase(std::remove_if(m_assimilators.begin(), m_assimilators.end(), [assimilator](const auto& p) {
        return p.first == assimilator;
    }), m_assimilators.end());
    LOG_DEBUG << "[BASES_MANAGER] Assimilator " << assimilator->getID() << " was removed from base on location " << this->getBaseLocation()->getBaseId() << ". Removed: " << (m_assimilators.size() < initialLength) << BOT_ENDL;
}

void Base::onDestroyed() {
    LOG_DEBUG << "[BASES_MANAGER] Base was destroyed on location:" << m_baseLocation->getBaseId() << BOT_ENDL;
    m_workers->deform();
}
