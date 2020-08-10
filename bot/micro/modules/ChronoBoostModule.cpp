#include "ChronoBoostModule.h"
#include "general/CCBot.h"

ChronoBoostModule::ChronoBoostModule(CCBot &bot): m_bot(bot) {}

void ChronoBoostModule::onFrame() {
    auto nexuses = m_bot.UnitInfo().getUnits(
            Players::Self,
            sc2::UNIT_TYPEID::PROTOSS_NEXUS
    );
    int chronoBoostsAvailable = 0;
    for (auto& x : nexuses) {
        chronoBoostsAvailable += ((int)x->getEnergy()) / 50;
    }
    sc2::Units nexusesRaw;
    for (auto x : nexuses) {
        nexusesRaw.push_back(x->getUnitPtr());
    }
    for (auto& x : nexuses) {
        if (chronoBoostsAvailable == 0) break;
        auto&& buffs = x->getUnitPtr()->buffs;
        if (buffs.empty() && x->isTraining()) {
            m_bot.Actions()->UnitCommand(
                    nexusesRaw,
                    sc2::ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST,
                    x->getUnitPtr(),
                    false
            );
            --chronoBoostsAvailable;
        }
    }
}
