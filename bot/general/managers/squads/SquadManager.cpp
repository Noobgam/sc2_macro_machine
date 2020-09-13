#include <util/Util.h>
#include "SquadManager.h"
#include "../../../util/LogInfo.h"
#include "../../CCBot.h"

SquadManager::SquadManager(CCBot & bot):
    m_bot(bot),
    m_squads() {
    unassignedSquadID = SquadManager::currentSquadID++;
    m_squads.insert({unassignedSquadID, std::make_unique<Squad>(bot, unassignedSquadID)});
}

void SquadManager::onStart() {
    for (const auto& unit : m_bot.UnitInfo().getUnits(Players::Self)) {
        addUnitCallback(unit);
    }
}

void SquadManager::onFrame() {
    for (auto & squad : m_squads) {
        if (squad.first != unassignedSquadID) {
            squad.second->act();
        }
        for (auto& unit : squad.second.get()->units()) {
            BOT_ASSERT(unit->isValid(), "Unit is invalid!");
        }
    }
}

Squad* SquadManager::getUnassignedSquad() const {
    return getSquad(unassignedSquadID).value();
}

std::optional<Squad*> SquadManager::getSquad(SquadID id) const {
    auto iter = m_squads.find(id);
    if (iter == m_squads.end()) {
        return {};
    }
    return iter->second.get();
}

Squad *SquadManager::getUnitSquad(const Unit *unit) const {
    auto squad = m_units.find(unit->getID());
    BOT_ASSERT(squad != m_units.end() && squad->second != nullptr, "Squad for unit not found");
    return squad->second;
}

void SquadManager::addUnitCallback(const Unit *unit) {
    if (unit->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPT)) {
        adeptsAndShades.push_back({unit, nullptr});
    } else if (unit->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT)) {
        // adept-shades are unique in that regard
        // always attempt to match it with corresponding adept
        // perhaps interceptors are to be ignored here if they are present in observation
        // because we cannot issue commands to them
        std::pair<const Unit*, const Unit*>* adeptShadeRef = nullptr;
        float distToClosestAdept = -1;
        for (auto&& adept : adeptsAndShades) {
            if (adept.second != nullptr) {
                continue;
            }
            float dist = Util::Dist(*unit, *adept.first);
            if (adeptShadeRef == nullptr || distToClosestAdept < dist) {
                adeptShadeRef = &adept;
                distToClosestAdept = dist;
            }
        }
        BOT_ASSERT(adeptShadeRef != nullptr, "No adept matching shade found");
        adeptShadeRef->second = unit;
        auto it = m_units.find(adeptShadeRef->first->getID());
        Squad* squad = it->second;
        squad->addUnit(unit);
        m_units.insert({unit->getID(), squad});
        return;
    }
    Squad* unassignedSquad = getUnassignedSquad();
    unassignedSquad->addUnit(unit);
    m_units.insert({ unit->getID(), unassignedSquad });
}

void SquadManager::removeUnitCallback(const Unit *unit) {
    Squad* squad = getUnitSquad(unit);
    m_units.erase(unit->getID());
    squad->removeUnits({unit});

    if (unit->getType().is(sc2::UNIT_TYPEID::PROTOSS_ADEPT)) {
        for (auto it = adeptsAndShades.begin(); it != adeptsAndShades.end(); ++it) {
            if (it->first == unit) {
                adeptsAndShades.erase(it);
                break;
            }
        }
    }
}

Squad* SquadManager::createNewSquad() {
    SquadID id = SquadManager::currentSquadID++;
    std::unique_ptr<Squad> squad = std::make_unique<Squad>(m_bot, id);
    Squad* res = squad.get();
    m_squads.insert({id, std::move(squad)});
    return res;
}

Squad* SquadManager::mergeSquads(std::vector<Squad*> & squads) {
    Squad* merged = createNewSquad();
    for (auto squad : squads) {
        transferUnits(squad, merged);
    }
    return merged;
}

void SquadManager::transferUnits(Squad* from, Squad* to) {
    LOG_DEBUG << "Transferring units from [" << from->m_id << "] to [ " << to->m_id << ']' << BOT_ENDL;
    if (from == to) {
        return;
    }
    const std::set<const Unit*>& units = from->units();
    to->addUnits(units);
    for (auto& unit : units) {
        DEBUG_ASSERT(unit->isValid(), "Transferred invalid unit");
        m_units.find(unit->getID())->second = to;
    }
    from->clear();
}

void SquadManager::transferUnits(const std::set<const Unit*> & units, Squad* to) {
    to->addUnits(units);
    std::vector<const Unit*> unitsCopy;
    unitsCopy.insert(unitsCopy.end(), units.begin(), units.end());
    for (auto unit : unitsCopy) {
        Squad* oldSquad = getUnitSquad(unit);
        if (oldSquad != to) {
            oldSquad->removeUnits({unit});
            m_units.find(unit->getID())->second = to;
        }
    }
}

void SquadManager::deformSquad(Squad* squad) {
    LOG_DEBUG << "[SQUAD_MANAGER] Deforming squad with id" << squad->getId() << BOT_ENDL;
    transferUnits(squad, getUnassignedSquad());
    if (squad->getId() != unassignedSquadID) {
        m_squads.erase(squad->getId());
    }
}
