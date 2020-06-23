#include "SquadManager.h"

SquadID SquadManager::currentSquadID = 0;

SquadManager::SquadManager():
    m_squads() {
    unassignedSquadID = SquadManager::currentSquadID++;
    m_squads.insert({SquadManager::currentSquadID++, std::make_shared<Squad>(unassignedSquadID)});
}

std::map<SquadID, std::shared_ptr<Squad>> SquadManager::getSquads() const {
    return m_squads;
}

Squad &SquadManager::getUnassignedSquad() const {
    return *getSquad(SquadManager::unassignedSquadID).value();
}

std::optional<std::shared_ptr<Squad>> SquadManager::getSquad(SquadID id) const {
    auto iter = m_squads.find(id);
    if (iter == m_squads.end()) {
        return {};
    }
    return iter->second;
}

SquadID SquadManager::createNewSquad() {
    SquadID id = SquadManager::currentSquadID++;
    m_squads.insert({SquadManager::currentSquadID++, std::make_shared<Squad>(id)});
    return currentSquadID;
}

SquadID SquadManager::mergeSquads(const std::vector<SquadID> & ids) {
    SquadID merged = createNewSquad();
    for (SquadID squadId : ids) {
        transferUnits(squadId, merged);
    }
    return merged;
}

void SquadManager::transferUnits(SquadID from, SquadID to) {
    auto fromOpt = getSquad(from);
    BOT_ASSERT(fromOpt.has_value(), "From squad not found");
    std::shared_ptr<Squad> fromSquad = fromOpt.value();

    // temporary solution
    auto toOpt = getSquad(from);
    BOT_ASSERT(toOpt.has_value(), "From squad not found");
    std::shared_ptr<Squad> toSquad = toOpt.value();
    toSquad->addUnits(fromSquad->units());
    fromSquad->clear();

    //    transferUnits(fromSquad->units(), to);
    if (to != SquadManager::unassignedSquadID && toSquad->isEmpty()) {
        m_squads.erase(to);
    }
}

void SquadManager::transferUnits(const std::set<Unit> & units, SquadID to) {
    auto toOpt = getSquad(to);
    BOT_ASSERT(toOpt.has_value(), "From squad not found");
    std::shared_ptr<Squad> toSquad = toOpt.value();
    toSquad->addUnits(units);
    // TODO remove units from map Unit -> Squad
}
