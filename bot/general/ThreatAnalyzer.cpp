#include "ThreatAnalyzer.h"

float ThreatAnalyzer::getUnitTypeThreat(const UnitType& attacker, const UnitType& attacked) {
    if (!attacked.is(sc2::UNIT_TYPEID::PROTOSS_STALKER)) {
        // no fucking idea at the moment
        return 0;
    }
    if (attacker.is(sc2::UNIT_TYPEID::PROTOSS_IMMORTAL)
        || attacker.is(sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED)
        || attacker.is(sc2::UNIT_TYPEID::TERRAN_SIEGETANK)
    ) {
        return 5;
    }
    if (attacker.is(sc2::UNIT_TYPEID::PROTOSS_VOIDRAY)) {
        return 4;
    }
    if (attacker.is(sc2::UNIT_TYPEID::PROTOSS_COLOSSUS) || attacker.is(sc2::UNIT_TYPEID::TERRAN_MARAUDER)) {
        return 3.5;
    }
    if (attacker.is(sc2::UNIT_TYPEID::PROTOSS_STALKER)) {
        return 3;
    }
    if (attacker.isCombatUnit() || attacker.is(sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON)) {
        return 2;
    }
    if (attacker.is(sc2::UNIT_TYPEID::PROTOSS_SHIELDBATTERY)) {
        return 1.5;
    }
    if (attacker.isWorker()) {
        return 1;
    }
    return 0;
}

// number of stalkers which it takes to kill a unit.
float ThreatAnalyzer::getUnitTypeThreat(const sc2::UnitTypeData &unit) {
#pragma region PROTOSS
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_IMMORTAL) {
        return 3.f;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_VOIDRAY) {
        return 2.6f;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_COLOSSUS) {
        return 2.1f;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_ADEPT) {
        // technically 1 stalker easykills adept, but it takes a really long time to do so.
        return 1.5f;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT) {
        // hard to estimate threat of an adept
        return 1.5f;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR) {
        return 1.5f;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_STALKER) {
        return 1;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON) {
        return 2;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_SHIELDBATTERY) {
        return 4;
    }
    if (unit.unit_type_id == sc2::UNIT_TYPEID::PROTOSS_PROBE) {
        // 4 probes kill a stalker if surrounded
        return 0.25f;
    }
#pragma endregion PROTOSS
#pragma region TERRAN
    if (unit.unit_type_id == sc2::UNIT_TYPEID::TERRAN_MARINE) {

    }
#pragma endregion TERRAN

    return 0;
}
