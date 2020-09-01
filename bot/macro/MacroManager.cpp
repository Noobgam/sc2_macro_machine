#include <util/Util.h>
#include <micro/order/workers/BuildingOrder.h>
#include "MacroManager.h"
#include "../general/CCBot.h"
#include "build_managers/SupplyBuildManager.h"
#include "build_managers/economy/EconomyBuildManager.h"
#include "build_managers/ProductionManager.h"
#include "build_managers/UnitHireManager.h"
#include "build_managers/TechBuildManager.h"
#include "../util/LogInfo.h"

MacroManager::MacroManager(CCBot & bot)
    : m_bot             (bot)
    , m_buildingPlacer (bot)
    , m_managers        ()
{
    m_managers.emplace_back(std::make_unique<SupplyBuildManager>(m_bot));
    m_managers.emplace_back(std::make_unique<EconomyBuildManager>(m_bot));
    m_managers.emplace_back(std::make_unique<ProductionManager>(m_bot));
    m_managers.emplace_back(std::make_unique<UnitHireManager>(m_bot));
    m_managers.emplace_back(std::make_unique<TechBuildManager>(m_bot));
}

void MacroManager::onStart() {
    m_buildingPlacer.onStart();
}

void MacroManager::onFrame(){
    static int frameId = 0;

    if (++frameId == 3) {
        LOG_DEBUG << "Getting top priority" << BOT_ENDL;
        std::optional<BuildOrderItem> item = getTopPriority();
        if (item.has_value()) {
            LOG_DEBUG << "Top priority item is " << item->type.getName() << " with priority " << item->priority << BOT_ENDL;
            produceIfPossible(item.value());
        } else {
            LOG_DEBUG << "No candidates to build" << BOT_ENDL;
        }
        frameId = 0;
    }

    drawProductionInformation();
}

std::optional<BuildOrderItem> MacroManager::getTopPriority() {
    std::vector<BuildOrderItem> items;
    for (const auto& manager : m_managers) {
        const auto& prio = manager->getTopPriority();
        if (prio.has_value()) {
            items.push_back(prio.value());
        }
    }

    std::stringstream ss;
    ss << "Production Information\n\n";

    for (auto& item : items) {
        ss << item.type.getName() << " : " << item.priority << '\n';
    }
    ss << '\n';
    ss << "Tasks in Queue:\n\n:";
    for (auto taskPtr : m_bot.getManagers().getBuildingManager().getTasks()) {
        ss << taskPtr->getType().getName() << " " << taskPtr->getStatus();
    }
    cachedProductionInformation = std::move(ss.str());

    auto item_ptr = std::max_element(items.begin(), items.end());
    if (item_ptr == items.end()) {
        return {};
    }


    return *item_ptr;
}

void MacroManager::produceIfPossible(BuildOrderItem item) {
    if (item.type.isBuilding()) {
        produceBuilding(item.type.getUnitType());
        return;
    }
    std::optional<const Unit*> producer = getProducer(item.type);
    if (producer.has_value() && canMakeNow(producer.value(), item.type)) {
        produce(producer.value(), item);
    }
}

std::optional<const Unit*> MacroManager::getProducer(const MetaType& type) {
    // get all the types of units that caa build this type
    auto& producerTypes = m_bot.Data(type).whatBuilds;

    for (auto unit : m_bot.UnitInfo().getUnits(Players::Self)) {
        // reasons a unit can not train the desired type
        if (std::find(producerTypes.begin(), producerTypes.end(), unit->getType()) == producerTypes.end()) { continue; }
        if (!unit->isCompleted()) { continue; }
        if (m_bot.Data(*unit).isBuilding && unit->isTraining()) { continue; }
        if (unit->isFlying()) { continue; }

        return unit;
    }
    
    // some invalid unit
    return {};
}

// this function will check to see if all preconditions are met and then create a unit
void MacroManager::produce(const Unit* producer, BuildOrderItem item) {
    BOT_ASSERT(!item.type.isBuilding(), "Building must be processed earlier");
    // if we're dealing with a non-building unit
    if (item.type.isUnit()) {
        if (producer->needsRallyPoint()) {
            if (producer->isOfType(sc2::UNIT_TYPEID::PROTOSS_NEXUS)) {
                Base* base = m_bot.getManagers().getBasesManager().findBaseByNexus(producer);
                if (base != nullptr) {
                    // mineral closest to center of resources
                    producer->rightClick(
                        base->getBaseLocation()->getPosition()
                    );
                }
            } else {
                int best = -1;
                std::optional<CCTilePosition> bestTile;
                int W = producer->getType().getFootPrintRadius() * 2 + .5;
                int x = producer->getPosition().x - producer->getType().getFootPrintRadius() - 1;
                int y = producer->getPosition().y - producer->getType().getFootPrintRadius() - 1;
                for (int cx = x; cx <= x + W + 1; ++cx) {
                    for (int cy = y; cy <= y + W + 1; ++cy) {
                        if (m_bot.Map().isWalkable(cx, cy)) {
                            int val = m_bot.Map().getSectorCnt(cx, cy);
                            if (best < val) {
                                best = val;
                                bestTile = {cx, cy};
                            }
                        }
                    }
                }
                if (bestTile.has_value()) {
                    float x = bestTile.value().x + .5;
                    float y = bestTile.value().y + .5;
                    producer->rightClick({x, y});
                }
            }
        }
        producer->train(item.type.getUnitType());
    }
    else if (item.type.isUpgrade())
    {
        // TODO: UPGRADES
        //Micro::SmartAbility(producer, m_bot.Data(item.type.getUpgradeID()).buildAbility, m_bot);
    }
}

bool MacroManager::canMakeNow(const Unit* producer, const MetaType & type) {
    if (!meetsReservedResources(type)) {
        return false;
    }

    sc2::AvailableAbilities available_abilities = m_bot.Query()->GetAbilitiesForUnit(producer->getUnitPtr());

    // quick check if the unit can't do anything it certainly can't build the thing we want
    if (available_abilities.abilities.empty())
    {
        return false;
    }
    else
    {
        // check to see if one of the unit's available abilities matches the build ability type
        sc2::AbilityID MetaTypeAbility = m_bot.Data(type).buildAbility;
        for (const sc2::AvailableAbility & available_ability : available_abilities.abilities)
        {
            if (available_ability.ability_id == MetaTypeAbility)
            {
                return true;
            }
        }
    }

    return false;
}

int MacroManager::getFreeMinerals()
{
    return m_bot.GetMinerals();
}

int MacroManager::getFreeGas()
{
    return m_bot.GetGas();
}

bool MacroManager::meetsReservedResources(const MetaType & type) {
    int mineralCost = m_bot.Data(type).mineralCost;
    int vespeneCost = m_bot.Data(type).gasCost;
    auto& economyManager = m_bot.getManagers().getEconomyManager();
    return mineralCost <= economyManager.getAvailableResources(ResourceType::MINERAL) &&
            vespeneCost <= economyManager.getAvailableResources(ResourceType::VESPENE);
}

void MacroManager::drawProductionInformation() {
    BRK_IF_NOT_DEBUG
    m_bot.Map().drawTextScreen(0.01f, 0.01f, cachedProductionInformation, CCColor(255, 255, 0));
    m_buildingPlacer.drawReservedTiles();
}

BuildingPlacer &MacroManager::getBuildingPlacer() {
    return m_buildingPlacer;
}

void MacroManager::produceBuilding(const UnitType& buildingType) {
    // find build position
    Timer timer;
    timer.start();
    std::optional<CCPosition> positionOpt = m_buildingPlacer.getBuildLocation(buildingType);
    if (!positionOpt.has_value()) {
        LOG_DEBUG << "Build place search failed. Took` " << timer.getElapsedTimeInMilliSec() << "ms" << BOT_ENDL;
        return;
    }
    LOG_DEBUG << "Build place search took " << timer.getElapsedTimeInMilliSec() << "ms" << BOT_ENDL;
    CCPosition position = positionOpt.value();

    // find builder
    auto freeWorkers = m_bot.getManagers().getWorkerManager().getFreeWorkers();
    if (freeWorkers.empty()) {
        LOG_DEBUG << "[SURRENDER_REQUEST] No workers found."<< BOT_ENDL;
        return;
    }
    const Unit *worker = nullptr;
    for (auto w : freeWorkers) {
        if (worker == nullptr || (Util::Dist(w, position) < Util::Dist(worker, position))) {
            worker = w;
        }
    }

    // estimate resources
    int mineralCost = m_bot.Data(buildingType).mineralCost;
    int vespeneCost = m_bot.Data(buildingType).gasCost;
    float seconds = m_bot.Map().getWalkTime(worker, position);
    if (
        m_bot.getManagers().getEconomyManager().getAvailableResources(ResourceType::MINERAL, seconds) >= mineralCost &&
        m_bot.getManagers().getEconomyManager().getAvailableResources(ResourceType::VESPENE, seconds) >= vespeneCost
    ) {
        m_buildingPlacer.reserveTiles(buildingType, position);
        BuildingTask* task = m_bot.getManagers().getBuildingManager().newTask(buildingType, worker, position);
        Squad* squad = m_bot.getManagers().getWorkerManager().formSquad({worker});
        const auto& buildOrder = std::make_shared<BuildingOrder>(m_bot, squad, task);
        squad->setOrder(buildOrder);
    }
}
