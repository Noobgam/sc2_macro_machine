#include "MacroManager.h"
#include "../general/CCBot.h"
#include "build_managers/SupplyBuildManager.h"
#include "build_managers/economy/EconomyBuildManager.h"
#include "build_managers/ProductionManager.h"
#include "build_managers/UnitHireManager.h"
#include "build_managers/TechBuildManager.h"
#include "../util/LogInfo.h"

using BOT_ENDL;

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
    cachedProductionInformation = std::move(ss.str());

    auto item_ptr = std::max_element(items.begin(), items.end());
    if (item_ptr == items.end()) {
        return {};
    }
    return *item_ptr;
}

void MacroManager::produceIfPossible(BuildOrderItem item) {
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
    // if we're dealing with a building
    const UnitType &buildingType = item.type.getUnitType();
    if (item.type.isBuilding()) {
        Timer timer;
        timer.start();
        std::optional<CCPosition> positionOpt = m_buildingPlacer.getBuildLocation(buildingType);
        if (!positionOpt.has_value()) {
            LOG_DEBUG << "Build place search failed. Took` " << timer.getElapsedTimeInMilliSec() << "ms" << BOT_ENDL;
            return;
        }
        LOG_DEBUG << "Build place search took " << timer.getElapsedTimeInMilliSec() << "ms" << BOT_ENDL;
        CCPosition position = positionOpt.value();
        m_buildingPlacer.reserveTiles(buildingType, position);
        if (!m_bot.getManagers().getWorkerManager().build(buildingType, position)) {
            m_buildingPlacer.freeTiles(buildingType, position);
        }
    }
    // if we're dealing with a non-building unit
    else if (item.type.isUnit())
    {
        producer->train(buildingType);
    }
    else if (item.type.isUpgrade())
    {
        // TODO: UPGRADES
        //Micro::SmartAbility(producer, m_bot.Data(item.type.getUpgradeID()).buildAbility, m_bot);
    }
}

bool MacroManager::canMakeNow(const Unit* producer, const MetaType & type)
{
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

// return whether or not we meet resources, including building reserves
bool MacroManager::meetsReservedResources(const MetaType & type)
{
    // return whether or not we meet the resources
    int minerals = m_bot.Data(type).mineralCost;
    int gas = m_bot.Data(type).gasCost;

    return (m_bot.Data(type).mineralCost <= getFreeMinerals()) && (m_bot.Data(type).gasCost <= getFreeGas());
}

void MacroManager::drawProductionInformation() {
    m_bot.Map().drawTextScreen(0.01f, 0.01f, cachedProductionInformation, CCColor(255, 255, 0));
    m_buildingPlacer.drawReservedTiles();
}

BuildingPlacer &MacroManager::getBuildingPlacer() {
    return m_buildingPlacer;
}
