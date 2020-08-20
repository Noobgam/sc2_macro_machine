#include <micro/order/scouting/ScoutEnemyBasesOrder.h>
#include <util/LogInfo.h>
#include "ScoutModule.h"
#include "general/CCBot.h"

ScoutModule::ScoutModule(CCBot &bot) : m_bot(bot) {}

void ScoutModule::onFrame() {
    // clear squad if it is empty
    if (m_basesScoutID.has_value()) {
        const auto& squad = m_bot.getManagers().getSquadManager().getSquad(m_basesScoutID.value());
        if (!squad.has_value()) {
            LOG_DEBUG << "[SCOUT_MODULE] Scout squad was deformed. Squad id:" << squad.value()->getId() << BOT_ENDL;
            m_basesScoutID = {};
        } else if (squad.value()->isEmpty()) {
            LOG_DEBUG << "[SCOUT_MODULE] Scout squad is empty: Squad id:" << m_basesScoutID.value() << BOT_ENDL;
            m_basesScoutID = {};
        }
    }
    // make scout squad if none currently
    if (!m_basesScoutID.has_value()) {
        const auto & bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getExpectedEnemyBaseLocations();
        if (!bases.empty()) {
            const auto& squad = m_bot.getManagers().getWorkerManager().formSquad(1);
            LOG_DEBUG << "[SCOUT_MODULE] New scout squad was formed: " << (squad.has_value() ? std::to_string(squad.value()->getId()) : "None") << BOT_ENDL;
            if (squad.has_value()) {
                std::stringstream ss;
                ss << "[SCOUT_MODULE] Setting scout order for squad " << squad.value()->getId() << ". Checking bases: ";
                for (const auto& base : bases) {
                    ss << base->getBaseId() << " ";
                }
                LOG_DEBUG << ss.str() << BOT_ENDL;
                m_basesScoutID = squad.value()->getId();
            }
        }
    }
}
