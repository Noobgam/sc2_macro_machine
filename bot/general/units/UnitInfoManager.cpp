#include "UnitInfoManager.h"
#include "../CCBot.h"
#include "../../util/LogInfo.h"

#include <sstream>

UnitInfoManager::UnitInfoManager(CCBot & bot) : m_bot(bot) {
    m_units.insert({Players::Self, {}});
    m_units.insert({Players::Enemy, {}});
    m_units.insert({Players::Neutral, {}});
}

void UnitInfoManager::onStart() {
    updateUnits();
}

void UnitInfoManager::onFrame() {
    updateUnits();
}

void UnitInfoManager::updateUnits() {
    size_t observationId = m_bot.getObservationId();
    for (auto & unit : m_bot.Observation()->GetUnits()) {
        auto it = unitWrapperByTag.find(unit->tag);
        if (it == unitWrapperByTag.end()) {
            auto inserted = unitWrapperByTag.insert({unit->tag, std::make_unique<Unit>(unit, m_bot, observationId)});
            processNewUnit(inserted.first->second.get());
        } else {
            it->second->updateObservationId(observationId);
        }
    }

    std::vector<std::unique_ptr<Unit>> missingUnits;
    for (auto it = unitWrapperByTag.begin(); it != unitWrapperByTag.end(); ) {
        bool notObserved = it->second->getObservationId() != observationId;
        bool dead = !it->second->isAlive();
        bool needToDelete = notObserved || dead;
        if (needToDelete) {
            std::cerr << "Unit is missing. [" << it->second->getType().getName() << "]" << std::endl;
            missingUnits.push_back(std::move(it->second));
            it = unitWrapperByTag.erase(it);
        } else {
            ++it;
        }
    }

    // callback missingUnits before destruction
    if (missingUnits.size() > 0) {
        std::cerr << missingUnits.size() << " < " << std::endl;
    }
    for (auto & unit : missingUnits) {
        processRemoveUnit(unit.get());
    }

    // update units map
    m_units.find(Players::Self)->second.clear();
    m_units.find(Players::Enemy)->second.clear();
    m_units.find(Players::Neutral)->second.clear();
    for (const auto & it : unitWrapperByTag) {
        Unit* unit = it.second.get();
        CCPlayer owner = unit->getPlayer();
        m_units.find(owner)->second.push_back(unit);
    }
}

void UnitInfoManager::processNewUnit(const Unit* unit) {
    updateSquadsWithNewUnit(unit);
    m_bot.getManagers().getBuildingManager().newUnitCallback(unit);
}

void UnitInfoManager::updateSquadsWithNewUnit(const Unit *unit) {
    if (unit->getPlayer() == Players::Self && unit->getType().isRegularUnit()) {
        m_bot.getManagers().getSquadManager().addUnitCallback(unit);
    }
}

void UnitInfoManager::processRemoveUnit(const Unit* unit) {
    updateSquadsWithRemovedUnit(unit);
    m_bot.getManagers().getBuildingManager().unitDiedCallback(unit);
}

void UnitInfoManager::updateSquadsWithRemovedUnit(const Unit *unit) {
    if (unit->getPlayer() == Players::Self && unit->getType().isRegularUnit()) {
        m_bot.getManagers().getSquadManager().removeUnitCallback(unit);
    }
}

const std::vector<const Unit*> & UnitInfoManager::getUnits(CCPlayer player) const {
    BOT_ASSERT(m_units.find(player) != m_units.end(), "Couldn't find player units: %d", player);
    return m_units.at(player);
}

// passing in a unit type of 0 returns a count of all units
size_t UnitInfoManager::getUnitTypeCount(CCPlayer player, UnitType type, bool completed) const {
    size_t count = 0;

    for (auto & unit : getUnits(player)) {
        if ((!type.isValid() || type == unit->getType()) && (!completed || unit->isCompleted())) {
            count++;
        }
    }

    return count;
}

int UnitInfoManager::getBuildingCount(CCPlayer player, UnitType type, UnitStatus status) const {
    int count = 0;
    for (auto & unit : getUnits(player)) {
        if (type != unit->getType()) {
            continue;
        }
        if (status & UnitStatus::COMPLETED && unit->isCompleted()) {
            if (status & UnitStatus::NOT_TRAINING) {
               if (!unit->isTraining()) {
                   count++;
               }
            } else {
                count++;
            }
            continue;
        }
    }
    for (const auto & task : m_bot.getManagers().getBuildingManager().getTasks()) {
        if (task->getType() == type) {
            if (status & UnitStatus::ORDERED && task->getStatus() == BuildingStatus::NEW) {
                count++;
                continue;
            }
            if (status & UnitStatus::CONSTRUCTING && task->getStatus() == BuildingStatus::IN_PROGRESS) {
                count++;
                continue;
            }
        }
    }
    return count;
}
