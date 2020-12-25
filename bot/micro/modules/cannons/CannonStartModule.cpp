#include "CannonStartModule.h"

#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <micro/order/scouting/ScoutEnemyBaseOrder.h>

CannonStartModule::CannonStartModule(CCBot &bot)
    : m_bot(bot)
{

}

void CannonStartModule::onFrame() {
    // make scout squad if none currently
    if (!m_mainScoutID.has_value()) {
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
                m_mainScoutID = squad.value()->getId();
                squad.value()->setOrder(std::make_shared<ScoutEnemyBaseOrder>(m_bot, squad.value(), bases[0]));
            }
        }
    }

}
