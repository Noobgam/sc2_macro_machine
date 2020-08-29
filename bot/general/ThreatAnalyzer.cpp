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
