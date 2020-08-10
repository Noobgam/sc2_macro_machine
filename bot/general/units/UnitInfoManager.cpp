#include "UnitInfoManager.h"
#include "../CCBot.h"
#include "../../util/LogInfo.h"

#include <sstream>
#include <util/Util.h>

UnitInfoManager::UnitInfoManager(CCBot & bot) : m_bot(bot) {
    m_units.insert({Players::Self, {}});
    m_units.insert({Players::Enemy, {}});
    m_units.insert({Players::Neutral, {}});
}

void UnitInfoManager::onStart() {
    VALIDATE_CALLED_ONCE();

    // collect initial units
    size_t observationId = m_bot.getObservationId();
    for (auto & rawUnit : m_bot.Observation()->GetUnits()) {
        const auto& it = unitWrapperByTag.insert({rawUnit->tag, std::make_unique<Unit>(rawUnit, m_bot, observationId)});
        const auto& unit = it.first->second.get();
        m_units.find(unit->getPlayer())->second.push_back(unit);
    }
}

void UnitInfoManager::onFrame() {
    updateUnits();
    auto&& units = m_units.find(Players::Self)->second;
    for (auto& unit : units) {
        auto&& centerPos = unit->getPosition();
        std::stringstream ss;
        ss << "[" << centerPos.x << ":" << centerPos.y << "]";
        m_bot.Debug()->DebugTextOut(ss.str(), unit->getUnitPtr()->pos);
    }
}

void UnitInfoManager::updateUnits() {
    size_t observationId = m_bot.getObservationId();
    for (auto & unit : m_bot.Observation()->GetUnits()) {
        auto it = unitWrapperByTag.find(unit->tag);
        if (it == unitWrapperByTag.end()) {
            if (unit->display_type == sc2::Unit::DisplayType::Placeholder) {
                BOT_ASSERT(unit->tag == 0, "Placeholder has id not equal to 0");
                continue;
            }
            auto inserted = unitWrapperByTag.insert({unit->tag, std::make_unique<Unit>(unit, m_bot, observationId)});
            processNewUnit(inserted.first->second.get());
        } else {
            it->second->updateObservationId(observationId);
        }
    }

    std::vector<std::unique_ptr<Unit>> missingUnits;
    for (auto it = unitWrapperByTag.begin(); it != unitWrapperByTag.end(); ) {
        bool notObserved = it->second->getObservationId() != observationId;
        if (notObserved) {
            auto& unit = it->second;
            if (unit->getPlayer() == Players::Self && unit->isAlive()) {
                ++it;
            } else {
                LOG_DEBUG << "Unit is missing. [" << it->second->getType().getName() << "]" <<  it->second.get()->isAlive() << " " << std::endl;
                missingUnits.push_back(std::move(it->second));
                it = unitWrapperByTag.erase(it);
            }
        } else {
            ++it;
        }
    }

    // callback missingUnits before destruction
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
    m_bot.getManagers().getResourceManager().newUnitCallback(unit);
    m_bot.getManagers().getBuildingManager().newUnitCallback(unit);
    m_bot.getManagers().getBasesManager().newUnitCallback(unit);
}

void UnitInfoManager::updateSquadsWithNewUnit(const Unit *unit) {
    if (unit->getPlayer() == Players::Self && unit->getType().isRegularUnit()) {
        m_bot.getManagers().getSquadManager().addUnitCallback(unit);
    }
}

void UnitInfoManager::processRemoveUnit(const Unit* unit) {
    updateSquadsWithRemovedUnit(unit);
    m_bot.getManagers().getResourceManager().unitDisappearedCallback(unit);
    m_bot.getManagers().getBuildingManager().unitDisappearedCallback(unit);
    m_bot.getManagers().getBasesManager().unitDisappearedCallback(unit);
    m_bot.Commander().getMacroManager().getBuildingPlacer().unitDisappearedCallback(unit);
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

std::vector<const Unit*> UnitInfoManager::getUnits(CCPlayer player, sc2::UnitTypeID type) const {
    std::vector<const Unit*> unitsOfType;
    for (auto & unit : getUnits(player)) {
        if (type == unit->getType().getAPIUnitType() && unit->isCompleted()) {
            unitsOfType.push_back(unit);
        }
    }
    return unitsOfType;
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
