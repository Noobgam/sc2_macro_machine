#include "CannonStartModule.h"

#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <micro/order/scouting/ScoutEnemyBaseOrder.h>

CannonStartModule::CannonStartModule(CCBot &bot)
    : m_bot(bot)
{

}

std::optional<Squad*> CannonStartModule::assignScoutSquad(const BaseLocation* baseLocation) {
    const auto& squad = m_bot.getManagers().getWorkerManager().formSquad(1);
    LOG_DEBUG << "[SCOUT_MAIN_MODULE] New scout squad was formed: " << (squad.has_value() ? std::to_string(squad.value()->getId()) : "None") << BOT_ENDL;
    if (squad.has_value()) {
        std::stringstream ss;
        ss << "[SCOUT_MAIN_MODULE] Setting scout order for squad " << squad.value()->getId() << ". Checking bases: " << baseLocation->getBaseId();
        LOG_DEBUG << ss.str() << BOT_ENDL;
        squad.value()->setOrder(std::make_shared<ScoutEnemyBaseOrder>(m_bot, squad.value(), baseLocation));
    }
    return squad;
}

void CannonStartModule::onFrame() {
    // make scout squad if none currently
    auto bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getOccupiedEnemyBaseLocations();
    if (bases.empty()) {
        bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getExpectedEnemyBaseLocations();
    }
    if (bases.empty()) {
        return;
    }
    if (!m_mainScoutID.has_value()) {
        auto&& squad = assignScoutSquad(bases[0]);
        if (squad.has_value()) {
            m_mainScoutID = squad.value()->getId();
        }
        return;
    } else {
        if (!m_subScoutID.has_value()) {
            auto forgeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot);
            int forgeNumber = m_bot.UnitInfo().getBuildingCount(Players::Self, forgeType, UnitStatus::TOTAL);
            if (forgeNumber >= 1) {
                auto&& squad = assignScoutSquad(bases[0]);
                if (squad.has_value()) {
                    m_subScoutID = squad.value()->getId();
                }
            }
            return;
        }
    }

    static int cnt = 0;
    static boolean everAnalyzed = false;
    if (++cnt > 200 || needRecalculation) {
        analyzer.recalculate(m_bot);
        analyzer.analyzeAsync(bases[0]);
        cnt = 0;
        needRecalculation = false;
    }

    if (analyzer.getAnalysisRevision() != 0) {
        for (auto &&tile : analyzer.relevantTiles) {
            float x = tile.x;
            float y = tile.y;
            m_bot.Map().drawBox({x + .1f, y + .1f}, {x + 1.9f, y + 1.9f}, Colors::Green);
        }
    }
}

void CannonStartModule::newUnitCallback(const Unit *unit) {
    if (unit->getType().isBuilding()) {
        needRecalculation = true;
    }
}
