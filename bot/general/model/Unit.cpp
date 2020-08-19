#include <util/LogInfo.h>
#include "Unit.h"
#include "../CCBot.h"
#include "../../util/Util.h"

Unit::Unit(const sc2::Unit * unit, CCBot & bot, size_t observationId)
    : m_bot(&bot)
    , m_unit(unit)
    , m_unitID(unit->tag)
    , m_unitType(unit->unit_type, bot)
    , observationId(observationId)
{ }

const sc2::Unit * Unit::getUnitPtr() const {
    return m_unit;
}

void Unit::updateObservationId(size_t observationId) {
    this->observationId = observationId;
}

size_t Unit::getObservationId() const {
    return observationId;
}

const sc2::UnitTypeID & Unit::getAPIUnitType() const {
    BOT_ASSERT(isValid(), "Unit is not valid");
    return m_unit->unit_type;
}

bool Unit::operator < (const Unit & rhs) const {
    return m_unit < rhs.m_unit;
}

bool Unit::operator == (const Unit & rhs) const {
    return m_unit == rhs.m_unit;
}

const UnitType & Unit::getType() const {
    return m_unitType;
}


CCPosition Unit::getPosition() const {
    return m_unit->pos;
}

CCTilePosition Unit::getTilePosition() const {
    return Util::GetTilePosition(m_unit->pos);
}

CCHealth Unit::getHitPoints() const {
    return m_unit->health;
}

CCHealth Unit::getShields() const {
    return m_unit->shield;
}

CCHealth Unit::getEnergy() const {
    return m_unit->energy;
}

float Unit::getBuildPercentage() const {
    return m_unit->build_progress;
}

CCPlayer Unit::getPlayer() const {
    if (m_unit->alliance == sc2::Unit::Alliance::Self) { return 0; }
    else if (m_unit->alliance == sc2::Unit::Alliance::Enemy) { return 1; }
    else { return 2; }
}

CCUnitID Unit::getID() const {
    CCUnitID id = m_unit->tag;
    if (id != m_unitID) {
        LOG_DEBUG << "Unit ID changed from " << m_unitID << " to " << id << BOT_ENDL;
    }
    BOT_ASSERT(id == m_unitID, "Unit ID changed somehow");
    return id;
}

bool Unit::isCompleted() const {
    return m_unit->build_progress >= 1.0f;
}

bool Unit::isTraining() const {
    return m_unit->orders.size() > 0;
}

bool Unit::isBeingConstructed() const {
    return !isCompleted() && m_unit->build_progress > 0.0f;
}

float Unit::getWeaponCooldown() const {
    return m_unit->weapon_cooldown;
}

bool Unit::isCloaked() const {
    return m_unit->cloak;
}

bool Unit::isFlying() const {
    return m_unit->is_flying;
}

bool Unit::isAlive() const {
    return m_unit->is_alive;
}

bool Unit::isPowered() const {
    return m_unit->is_powered;
}

bool Unit::isIdle() const {
    return m_unit->orders.empty();
}

bool Unit::isBurrowed() const {
    return m_unit->is_burrowed;
}

bool Unit::isValid() const {
    return m_unit != nullptr;
}

void Unit::stop() const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, sc2::ABILITY_ID::STOP);
}

void Unit::attackUnit(const Unit & target) const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, sc2::ABILITY_ID::ATTACK_ATTACK, target.getUnitPtr());
}

void Unit::attackMove(const CCPosition & targetPosition) const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, sc2::ABILITY_ID::ATTACK_ATTACK, targetPosition);
}

void Unit::move(const CCPosition & targetPosition) const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, sc2::ABILITY_ID::MOVE_MOVE, targetPosition);
}

void Unit::move(const CCTilePosition & targetPosition) const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, sc2::ABILITY_ID::MOVE_MOVE, CCPosition((float)targetPosition.x, (float)targetPosition.y));
}

void Unit::rightClick(const Unit & target) const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, sc2::ABILITY_ID::SMART, target.getUnitPtr());
}

void Unit::repair(const Unit & target) const {
    rightClick(target);
}

void Unit::build(const UnitType & buildingType, CCPosition pos) const {
    if (!m_bot->Map().isConnected(getTilePosition(), pos)) {
        LOG_DEBUG << pos.x << " " << pos.y << BOT_ENDL;
        BOT_ASSERT(false, "Error: Build Position is not connected to worker");
    }
    m_bot->getUnitCommandManager().UnitCommand(m_unit, m_bot->Data(buildingType).buildAbility, pos);
}

void Unit::buildTarget(const UnitType & buildingType, const Unit & target) const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, m_bot->Data(buildingType).buildAbility, target.getUnitPtr());
}

void Unit::train(const UnitType & type) const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, m_bot->Data(type).buildAbility);
}

void Unit::morph(const UnitType & type) const {
    m_bot->getUnitCommandManager().UnitCommand(m_unit, m_bot->Data(type).buildAbility);
}

bool Unit::isConstructing(const UnitType & type) const {
    sc2::AbilityID buildAbility = m_bot->Data(type).buildAbility;
    return (getUnitPtr()->orders.size() > 0) && (getUnitPtr()->orders[0].ability_id == buildAbility);
}

bool Unit::isOfType(const sc2::UNIT_TYPEID& type) const {
    return this->getAPIUnitType() == type;
}

float Unit::hpPercentage() const {
    return m_unit->health / m_unit->health_max;
}

float Unit::shieldPercentage() const {
    return m_unit->shield / m_unit->shield_max;
}
