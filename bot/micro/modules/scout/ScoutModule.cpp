#include <micro/order/scouting/ScoutEnemyBasesOrder.h>
#include "ScoutModule.h"
#include "general/CCBot.h"
#include "logging/Events.h"

ScoutModule::ScoutModule(CCBot &bot) : m_bot(bot) {}

void ScoutModule::onFrame() {
    // clear squad if it is empty
    if (m_basesScoutID.has_value()) {
        const auto& squad = m_bot.getManagers().getSquadManager().getSquad(m_basesScoutID.value());
        if (!squad.has_value()) {
            events::scout_module::scoutSquadWasDeformed(m_basesScoutID.value()).log();
            m_basesScoutID = {};
        } else if (squad.value()->isEmpty()) {
            events::scout_module::scoutSquadIsEmpty(m_basesScoutID.value()).log();
            m_basesScoutID = {};
        }
    }
    // make scout squad if none currently
    if (!m_basesScoutID.has_value()) {
        const auto & bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getExpectedEnemyBaseLocations();
        if (!bases.empty()) {
            const auto& squad = m_bot.getManagers().getWorkerManager().formSquad(1);
            events::scout_module::newScoutSquadFormed(squad).log();
            if (squad.has_value()) {
                events::scout_module::setScoutBasesOrder(squad.value(), bases).log();
                squad.value()->setOrder(std::make_shared<ScoutEnemyBasesOrder>(m_bot, squad.value(), bases));
                m_basesScoutID = squad.value()->getId();
            }
        }
    }
}