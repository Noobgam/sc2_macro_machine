#include "SquadManager.h"
#include "../../../util/LogInfo.h"
#include "../../CCBot.h"

SquadManager::SquadManager(CCBot & bot):
    m_bot(bot),
    m_squads() {
    unassignedSquadID = SquadManager::currentSquadID++;
    m_squads.insert({unassignedSquadID, std::make_unique<Squad>(bot, unassignedSquadID)});
}

void SquadManager::onFrame() {
    for (auto & squad : m_squads) {
        if (squad.first != SquadManager::unassignedSquadID) {
            squad.second->act();
        }
    }
}

Squad* SquadManager::getUnassignedSquad() const {
    return getSquad(SquadManager::unassignedSquadID).value();
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
    Squad* unassignedSquad = getUnassignedSquad();
    unassignedSquad->addUnits({ unit });
    m_units.insert({ unit->getID(), getUnassignedSquad() });
}

void SquadManager::removeUnitCallback(const Unit *unit) {
    Squad* squad = getUnitSquad(unit);
    m_units.erase(unit->getID());
    squad->removeUnits({unit});
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
    LOG_DEBUG << "Transferring units" << std::endl;
    if (from == to) {
        return;
    }
    const std::set<const Unit*>& units = from->units();
    to->addUnits(units);
    for (auto unit : units) {
        m_units.find(unit->getID())->second = to;
    }
    from->clear();
    if (from->getId() != SquadManager::unassignedSquadID) {
        m_squads.erase(from->getId());
    }
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
    transferUnits(squad, getUnassignedSquad());
    if (squad->getId() != SquadManager::unassignedSquadID && squad->isEmpty()) {
        m_squads.erase(squad->getId());
    }
}
