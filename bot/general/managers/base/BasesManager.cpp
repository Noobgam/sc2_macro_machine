#include "BasesManager.h"
#include <general/bases/BaseLocation.h>
#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <util/Util.h>

BasesManager::BasesManager(CCBot &bot) : m_bot(bot) { }

void BasesManager::onStart() {
    VALIDATE_CALLED_ONCE();
    for (const auto& unit : m_bot.UnitInfo().getUnits(Players::Self)) {
        newUnitCallback(unit);
    }
}

void BasesManager::newUnitCallback(const Unit *unit) {
    if (unit->getPlayer() != Players::Self) {
        return;
    }
    // occupy new base
    if (unit->getType().isResourceDepot()) {
        const auto& baseLocations = m_bot.Bases().getBaseLocations();
        for (const auto& baseLocation : baseLocations) {
            if (baseLocation->getDepotActualPosition() == unit->getPosition()) {
                newBaseOccupied(baseLocation, unit);
            }
        }
    }
    // assimilator was build
    if (unit->getType().isRefinery()) {
        for (auto & base: m_basesPtrs) {
            tryAddAssimilator(base, unit);
        }
    }
}

void BasesManager::unitDisappearedCallback(const Unit *unit) {
    if (unit->getPlayer() != Players::Self) {
        return;
    }
    // assimiltor destroyed
    if (unit->getType().isRefinery()) {
        for (auto & base: m_basesPtrs) {
            const auto & assimilators = base->getAssimilators();
            const auto & it = std::find_if(assimilators.begin(), assimilators.end(), [unit](const auto& a) {
                return a.first == unit;
            });
            if (it != assimilators.end()) {
                base->onAssimilatorDestroyed(unit);
            }
        }
    }
    // base destroyed
    if (unit->getType().isResourceDepot()) {
        const auto& it = std::find_if(m_bases.begin(), m_bases.end(), [unit](const auto& b) {
            return b->getNexus() == unit;
        });
        if (it == m_bases.end()) {
            // some strange nexus
            return;
        }
        it->get()->onDestroyed();
        m_basesPtrs.erase(std::find(m_basesPtrs.begin(), m_basesPtrs.end(), it->get()));
        m_bases.erase(it);
    }
}

const std::vector<Base *> &BasesManager::getBases() const {
    return m_basesPtrs;
}

std::vector<Base *> BasesManager::getCompletedBases() const {
    std::vector<Base*> basesWithNexus;
    for (const auto & base : m_basesPtrs) {
        const auto& nexus = base->getNexus();
        if (nexus->isCompleted()) {
            basesWithNexus.push_back(base);
        }
    }
    return basesWithNexus;
}

void BasesManager::newBaseOccupied(const BaseLocation *baseLocation, const Unit * nexus) {
    LOG_DEBUG << "Occupying new base " << baseLocation->getBaseId() << " by unit " << nexus->getID() << BOT_ENDL;
    const auto& it = m_bases.emplace_back(std::make_unique<Base>(m_bot, baseLocation, nexus));
    const auto& base = it.get();
    m_basesPtrs.push_back(base);
    for (const auto& unit : m_bot.UnitInfo().getUnits(Players::Self)) {
        if (unit->getType().isRefinery()) {
            tryAddAssimilator(base, unit);
        }
    }
}

void BasesManager::tryAddAssimilator(Base * base, const Unit* unit) {
    const auto & geysers = base->getBaseLocation()->getGeysers();
    const auto & it = std::find_if(geysers.begin(), geysers.end(), [unit](const auto& g) {
        return g->getPosition() == unit->getPosition();
    });
    if (it != geysers.end()) {
        base->onNewAssimilator(unit, *it);
    }
}
bool BasesManager::isBaseOccupied(int baseId) const {
    for (auto x : m_basesPtrs) {
        if (x->getBaseLocation()->getBaseId() == baseId) {
            return true;
        }
    }
    return false;
}
