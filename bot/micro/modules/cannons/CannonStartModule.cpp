#include "CannonStartModule.h"

#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <micro/order/scouting/ScoutEnemyBaseOrder.h>
#include <random>

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
    if (++cnt > 200 || needRecalculation) {
        if (analyzer.recalculate(m_bot)) {
            needRecalculation = false;
            analyzer.analyzeAsync(bases[0]);
            cnt = 0;
        }
    }

    auto val = analyzer.latestAnalysis.exchange(NULL);
    if (val != NULL) {
        if (currentAnalysis != NULL) {
            delete currentAnalysis;
        }
        currentAnalysis = val;
        //std::random_shuffle(currentAnalysis->pylonPlacements.begin(), currentAnalysis->pylonPlacements.end());
    }
    if (currentAnalysis != NULL) {
        if (!currentAnalysis->pylonPlacements.empty()) {
            int id = rand() % currentAnalysis->pylonPlacements.size();
            auto it = currentAnalysis->pylonPlacements.begin();
            for (int i = 0; i < id; ++i) {
                ++it;
            }
            auto&& placement = *it;
            for (auto &&tile : placement.pylonPositions) {
                float x = tile.x;
                float y = tile.y;
                m_bot.Map().drawBox({x + .1f, y + .1f}, {x + 1.9f, y + 1.9f}, Colors::Green);
            }
//            for (int i = 0; i < m_bot.Map().width(); ++i) {
//                for (int j = 0; j < m_bot.Map().height(); ++j) {
//                    int compNum = placement.visitedSlow[i][j];
//                    if (compNum == 0) continue;
//                    if (compNum % 2 == 1) {
//                        m_bot.Map().drawTile(i, j, Colors::Purple);
//                    } else {
//                        m_bot.Map().drawTile(i, j, Colors::Red);
//                    }
//                }
//            }
        }
    }
}

void CannonStartModule::newUnitCallback(const Unit *unit) {
    if (unit->getType().isBuilding()) {
        needRecalculation = true;
    }
}
