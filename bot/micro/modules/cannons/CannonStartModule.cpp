#include "CannonStartModule.h"

#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <micro/order/scouting/ScoutEnemyBaseOrder.h>
#include <random>
#include <micro/order/cannons/FirstPylonPlacementOrder.h>

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
    if (!m_bot.getManagers().getSquadManager().validateSquadId(m_mainSquad)) {
        LOG_DEBUG << "Main squad was invalidated." << BOT_ENDL;
    }
    if (!m_bot.getManagers().getSquadManager().validateSquadId(m_subSquad)) {
        LOG_DEBUG << "Main squad was invalidated." << BOT_ENDL;
    }
    if (m_mainSquad.has_value()) {
        if (m_subSquad.has_value()) {
            m_mainSquad.swap(m_subSquad);
            return;
        }
        auto&& squadO = m_bot.getManagers().getSquadManager().getSquad(m_mainSquad.value());
        // if squad has been deformed, stop
        if (!squadO.has_value()) {
            m_mainSquad = {};
        }
    }

    // make scout squad if none currently
    auto bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getOccupiedEnemyBaseLocations();
    if (bases.empty()) {
        bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getExpectedEnemyBaseLocations();
    }
    if (bases.empty()) {
        return;
    }
    if (!m_mainSquad.has_value()) {
        if (m_subSquad.has_value()) {
            m_mainSquad.swap(m_subSquad);
            return;
        }
        auto&& squad = assignScoutSquad(bases[0]);
        if (squad.has_value()) {
            m_mainSquad = squad.value()->getId();
        }
        return;
    } else {
        if (!m_subSquad.has_value()) {
            auto forgeType = UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot);
            int forgeNumber = m_bot.UnitInfo().getBuildingCount(Players::Self, forgeType, UnitStatus::TOTAL);
            if (forgeNumber >= 1) {
                auto&& squad = assignScoutSquad(bases[0]);
                if (squad.has_value()) {
                    m_subSquad = squad.value()->getId();
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
            if (!selectedPlacement.has_value()) {
                srand(time(NULL));
                int id = rand() % currentAnalysis->pylonPlacements.size();
                auto it = currentAnalysis->pylonPlacements.begin();
                for (int i = 0; i < id; ++i) {
                    ++it;
                }
                selectedPlacement = *it;
            }
        }
    }
    auto mainSquad = m_bot.getManagers().getSquadManager().getSquad(m_mainSquad.value()).value();
    if (selectedPlacement.has_value()) {
        auto&& placement = selectedPlacement.value();
        for (auto &&tile : placement.pylonPositions) {
            float x = tile.x;
            float y = tile.y;
            m_bot.Map().drawBox({x + .1f, y + .1f}, {x + 1.9f, y + 1.9f}, Colors::Green);
        }
        for (auto &&tile : placement.cannonPlacements) {
            float x = tile.x;
            float y = tile.y;
            m_bot.Map().drawBox({x + .1f, y + .1f}, {x + 1.9f, y + 1.9f}, Colors::Purple);
        }
        if (m_bot.getManagers().getEconomyManager().getAvailableResources(ResourceType::MINERAL) > selectedPlacement.value().pylonPositions.size() * 100) {
            if (!mainSquad->hasOrder<cannons::FirstPylonPlacementOrder>()) {
                mainSquad->setOrder(
                        std::make_shared<cannons::FirstPylonPlacementOrder>(
                                m_bot,
                                mainSquad,
                                selectedPlacement.value()
                        )
                );
            }
        }
    }
}

void CannonStartModule::newUnitCallback(const Unit *unit) {
    if (unit->getType().isBuilding()) {
        needRecalculation = true;
        for (auto squadId : {m_mainSquad, m_subSquad}) {
            if (!squadId.has_value()) {
                continue;
            }
            auto squadO = m_bot.getManagers().getSquadManager().getSquad(squadId.value());
            if (squadO.has_value() && squadO.value()->hasOrder<cannons::FirstPylonPlacementOrder>()) {
                cannons::FirstPylonPlacementOrder* order =
                        dynamic_cast<cannons::FirstPylonPlacementOrder*>(squadO.value()->getOrder().get());
                order->processBuilding(unit);
            }
        }
    }
}
