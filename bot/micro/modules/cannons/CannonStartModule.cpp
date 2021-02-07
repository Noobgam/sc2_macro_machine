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
        if (analysisRev != latestProcessedRevision) {
            LOG_DEBUG << "Ready to change: " << analysisRev << " " << latestProcessedRevision << BOT_ENDL;
            std::vector<PylonPlacement> pylonPlacements;
            LOG_DEBUG << "Initial number of placements: " << currentAnalysis->pylonPlacements.size() << BOT_ENDL;
            for (auto&& pp : currentAnalysis->pylonPlacements) {
                if (isPlacementValid(pp) && isPylonPlacementScary(pp)) {
                    pylonPlacements.push_back(pp);
                } else {
                    if (!isPlacementValid(pp)) {
                        auto&& log = LOG_DEBUG << "Rejected invalid placement: ";
                        for (auto x : pp.pylonPositions) {
                            log << x.x << ":" << x.y << " ";
                        }
                        log << BOT_ENDL;
                    }
                    if (!isPylonPlacementScary(pp)) {
                        auto&& log = LOG_DEBUG << "Rejected non-scary placement: ";
                        for (auto x : pp.pylonPositions) {
                            log << x.x << ":" << x.y << " ";
                        }
                        log << BOT_ENDL;
                    }
                }
            }
            LOG_DEBUG << "First filter of placements: " << pylonPlacements.size() << BOT_ENDL;

            int mnPylonCount = 100;
            if (!pylonPlacements.empty()) {
                for (auto&& placement : pylonPlacements) {
                    mnPylonCount = std::min(mnPylonCount, static_cast<int>(placement.pylonPositions.size()));
                }
            }
            {
                std::vector<PylonPlacement> pylonPlacements2;
                for (auto&& placement : pylonPlacements) {
                    if (placement.pylonPositions.size() == mnPylonCount) {
                        pylonPlacements2.push_back(placement);
                    }
                }
                pylonPlacements.swap(pylonPlacements2);
            }
            LOG_DEBUG << "Selected only [" << mnPylonCount << "] pylon placements. " << pylonPlacements.size() << " left." << BOT_ENDL;

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
        if (!isPlacementValid(selectedPlacement.value())) {
            selectedPlacement = {};
            return;
        }
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
    std::vector<const Resource*> targetMinerals;

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
            targetMinerals.push_back(mineral);
        }
    }

    int goodCannons = 0;
    for (auto&& cannon : pylonPlacement.cannonPlacements) {
        const float PHOTON_CANNON_RANGE = 7.5;
        CCPosition cannonCenter{
                cannon.x + 1.f,
                cannon.y + 1.f
        };

        bool thisIsGood = false;

        for (auto&& unit : targets) {

            float scaryRange = PHOTON_CANNON_RANGE + unit->getUnitPtr()->radius - 0.1f;
            if (Util::Dist(unit, cannonCenter) <= scaryRange) {
                thisIsGood = true;
                break;
            }
        }

        if (thisIsGood) {
            ++goodCannons;
            continue;
        }

        for (auto&& mineral : targetMinerals) {
            float dist = Util::Dist(mineral->getUnit(), cannonCenter);
            float scaryRange = PHOTON_CANNON_RANGE - 1.f;
            if (dist <= scaryRange) {
                thisIsGood = true;
                break;
            }
        }
        if (thisIsGood) {
            ++goodCannons;
            continue;
        }
    }
    if (goodCannons == pylonPlacement.cannonPlacements.size()) {
        return true;
    }
    return false;
}

bool CannonStartModule::isPlacementValid(const PylonPlacement &placement) const {
    for (auto&& pylonPos : placement.pylonPositions) {
        for (int dx = 0; dx < 2; ++dx) {
            for (int dy = 0; dy < 2; ++dy) {
                int x = pylonPos.x + dx;
                int y = pylonPos.y + dy;
                if (!m_bot.Map().isBuildable(x, y)) {
                    return false;
                }
            }
        }
    }
    return true;
}
