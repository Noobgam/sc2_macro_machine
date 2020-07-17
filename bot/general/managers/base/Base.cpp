#include "Base.h"

Base::Base(CCBot &bot, const BaseLocation *baseLocation, const Unit *nexus):
    m_bot(bot),
    m_baseLocation(baseLocation),
    m_nexus(nexus),
    m_workers(std::make_unique<BaseWorkers>(bot, this))
{ }

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

std::vector<const Unit *> Base::getActiveAssimilators() const {
    std::vector<const Unit*> activeAssimilators;
    for (const auto& assimilator : m_assimilators) {
        if (assimilator.first->isCompleted() && assimilator.second->getResourceAmount() != 0) {
            activeAssimilators.emplace_back(assimilator.first);
        }
    }
    return activeAssimilators;
}

void Base::onNewAssimilator(const Unit *assimilator, const Resource *geyser) {
    m_assimilators.emplace_back(assimilator, geyser);
}

void Base::onAssimilatorDestroyed(const Unit *assimilator) {
    m_assimilators.erase(std::find_if(m_assimilators.begin(), m_assimilators.end(), [assimilator](const auto& p) {
        return p.first == assimilator;
    }));
}

void Base::onDestroyed() {
    m_workers->deform();
}
