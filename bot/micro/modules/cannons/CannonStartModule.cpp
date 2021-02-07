#include "CannonStartModule.h"

#include <general/CCBot.h>
#include <util/LogInfo.h>
#include <micro/order/scouting/ScoutEnemyBaseOrder.h>
#include <random>
#include <micro/order/cannons/FirstPylonPlacementOrder.h>
#include <util/Util.h>

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
    updateStrategy();
    if (strategy != Strategy::HighLevelStrategy::CANNONS) {
        return;
    }
    if (!m_bot.getManagers().getSquadManager().validateSquadId(m_mainSquad)) {
        LOG_DEBUG << "Main squad was invalidated." << BOT_ENDL;
    }
    if (!m_bot.getManagers().getSquadManager().validateSquadId(m_subSquad)) {
        LOG_DEBUG << "Main squad was invalidated." << BOT_ENDL;
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
        auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
        int pylonNumber = m_bot.UnitInfo().getBuildingCount(Players::Self, pylonType, UnitStatus::TOTAL);
        if (pylonNumber >= 1) {
            auto&& squad = assignScoutSquad(bases[0]);
            if (squad.has_value()) {
                m_mainSquad = squad.value()->getId();
            }
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
    } else {
        LOG_DEBUG << "Frame cycle: " << cnt << " - " << needRecalculation << BOT_ENDL;
    }

    auto val = analyzer.latestAnalysis.exchange(NULL);
    if (val != NULL) {
        currentAnalysis.reset(val);
        //std::random_shuffle(currentAnalysis->pylonPlacements.begin(), currentAnalysis->pylonPlacements.end());
    }
    if (currentAnalysis != NULL) {
        int analysisRev = analyzer.analysisRevision;
        LOG_DEBUG << "Ready to change: " << analysisRev << " " << latestProcessedRevision << BOT_ENDL;
        if (analysisRev != latestProcessedRevision) {
            std::vector<PylonPlacement> pylonPlacements;
            for (auto&& pp : currentAnalysis->pylonPlacements) {
                if (isPylonPlacementScary(pp)) {
                    pylonPlacements.push_back(pp);
                }
            }

            if (!pylonPlacements.empty()) {
                srand(time(NULL));
                int id = rand() % pylonPlacements.size();
                auto it = pylonPlacements.begin();
                for (int i = 0; i < id; ++i) {
                    ++it;
                }
                selectedPlacement = *it;
            } else {
                selectedPlacement = {};
                // TODO: there are no pylon placements which are valid. We need to change our strategy
                //  or cannon rush other base
            }
            latestProcessedRevision = analysisRev;
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

void CannonStartModule::updateStrategy() {
    if (strategy == m_bot.getStrategy().getCurrentStrategy()) {
        return;
    }
    strategy = m_bot.getStrategy().getCurrentStrategy();
    if (strategy != Strategy::HighLevelStrategy::CANNONS) {
        needRecalculation = false;
        currentAnalysis.reset();
        selectedPlacement = {};

        // we cannot deform squads because worker manager keeps track of completed orders.
        if (m_mainSquad.has_value()) {
            auto&& sqO = m_bot.getManagers().getSquadManager().getSquad(m_mainSquad.value());
            if (sqO.has_value()) {
                sqO.value()->getOrder()->onEnd();
            }
        }
        if (m_subSquad.has_value()) {
            auto&& sqO = m_bot.getManagers().getSquadManager().getSquad(m_subSquad.value());
            if (sqO.has_value()) {
                sqO.value()->getOrder()->onEnd();
            }
        }
        analyzer.requestCancel();
        return;
    }
}

bool CannonStartModule::isPylonPlacementScary(const PylonPlacement &pylonPlacement) const {
    auto&& enemies = m_bot.UnitInfo().getUnits(Players::Enemy);
    std::vector<const Unit*> targets;

    std::copy_if (enemies.begin(), enemies.end(), std::back_inserter(targets),
                  [](const Unit* unit) {
            return unit->getType().isBuilding();
        }
    );
//    auto&& minerals = m_bot.getManagers().getResourceManager().getMinerals();
//    for (auto&& mineral : minerals) {
//        targets.push_back(mineral->getUnit());
//    }

    for (auto&& occ : m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getOccupiedEnemyBaseLocations()) {
        for (auto&& mineral : occ->getMinerals()) {
            targets.push_back(mineral->getUnit());

        }
    }

    int goodCannons = 0;
    for (auto&& cannon : pylonPlacement.cannonPlacements) {
        for (auto&& unit : targets) {
            const int PHOTON_CANNON_RANGE = 7;
            CCPosition cannonCenter{
                cannon.x + 1.f,
                cannon.y + 1.f
            };
            float scaryRange =
                    unit->getType().isMineral()
                        ? PHOTON_CANNON_RANGE - 1.f
                        : PHOTON_CANNON_RANGE + unit->getUnitPtr()->radius - 0.1;
            if (Util::Dist(unit, cannonCenter) <= scaryRange) {
                ++goodCannons;
                break;
            }
        }
    }
    if (goodCannons == pylonPlacement.cannonPlacements.size()) {
        return true;
    }
    return false;
}
