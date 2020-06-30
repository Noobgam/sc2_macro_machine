#include "SquadManager.h"
#include "../../util/LogInfo.h"

SquadManager::SquadManager():
    m_squads() {
    unassignedSquadID = SquadManager::currentSquadID++;
    m_squads.insert({unassignedSquadID, std::make_unique<Squad>(unassignedSquadID)});
}

void SquadManager::removeUnitsFromSquad(const std::set<const Unit *> &units, Squad *squad) {
    squad->removeUnits(units);
    if (squad->getId() != SquadManager::unassignedSquadID && squad->isEmpty()) {
        m_squads.erase(squad->getId());
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
    auto squad = m_units.find(unit);
    BOT_ASSERT(squad != m_units.end() && squad->second != nullptr, "Squad for unit not found");
    return squad->second;
}

void SquadManager::addUnit(const Unit *unit) {
    Squad* unassignedSquad = getUnassignedSquad();
    unassignedSquad->addUnits({ unit });
    m_units.insert({ unit, getUnassignedSquad() });
}

void SquadManager::removeUnit(const Unit *unit) {
    Squad* squad = getUnitSquad(unit);
    m_units.erase(unit);
    removeUnitsFromSquad({unit}, squad);
}

Squad* SquadManager::createNewSquad() {
    SquadID id = SquadManager::currentSquadID++;
    std::unique_ptr<Squad> squad = std::make_unique<Squad>(id);
    Squad* res = squad.get();
    m_squads.insert({id, std::move(squad)});
    return res;
}

Squad* SquadManager::mergeSquads(std::vector<Squad*> & ids) {
    Squad* merged = createNewSquad();
    for (auto id : ids) {
        transferUnits(id, merged);
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
        m_units.find(unit)->second = to;
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
            removeUnitsFromSquad({ unit }, oldSquad);
            m_units.find(unit)->second = to;
        }
    }
}
