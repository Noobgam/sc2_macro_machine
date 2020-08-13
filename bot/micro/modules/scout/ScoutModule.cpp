#include <micro/order/attack/AttackWithKiting.h>
#include "ScoutModule.h"
#include "general/CCBot.h"

ScoutModule::ScoutModule(CCBot &bot) : m_bot(bot) {}

void ScoutModule::onFrame() {
    // clear squad if it is empty
    if (m_basesScoutID.has_value()) {
        const auto& squad = m_bot.getManagers().getSquadManager().getSquad(m_basesScoutID.value());
        if (!squad.has_value() || squad.value()->isEmpty()) {
            m_basesScoutID = {};
        }
    }
    if (!m_basesScoutID.has_value()) {
        const auto& squad = m_bot.getManagers().getWorkerManager().formSquad(1);
        if (squad.has_value()) {
            const auto & bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getExpectedEnemyBaseLocations();
            const auto& base = *bases.begin();
            squad.value()->setOrder(std::make_shared<AttackWithKiting>(m_bot, squad.value(), base->getPosition()));
            m_basesScoutID = squad.value()->getId();
        }
    }
}