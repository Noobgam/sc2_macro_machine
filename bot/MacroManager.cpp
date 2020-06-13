#include "MacroManager.h"
#include "Util.h"
#include "CCBot.h"
#include "SupplyBuildManager.h"
#include "WorkerBuildManager.h"
#include "ProductionManager.h"
#include "UnitHireManager.h"
#include "BuildManager.h"

MacroManager::MacroManager(CCBot & bot)
    : m_bot             (bot)
    , m_buildingManager (bot)
    , m_queue           (bot)
    , m_managers        ()
{
    m_managers.emplace_back(std::make_unique<SupplyBuildManager>(m_bot));
    m_managers.emplace_back(std::make_unique<WorkerBuildManager>(m_bot));
    m_managers.emplace_back(std::make_unique<ProductionManager>(m_bot));
    m_managers.emplace_back(std::make_unique<UnitHireManager>(m_bot));
}

void MacroManager::onStart()
{
      m_buildingManager.onStart();
//    setBuildOrder(m_bot.Strategy().getOpeningBookBuildOrder());
}

void MacroManager::onFrame()
{
//    fixBuildOrderDeadlock();
//    manageBuildOrderQueue();

    // TODO: if nothing is currently building, get a new goal from the strategy manager
    // TODO: detect if there's a build order deadlock once per second
    // TODO: triggers for game things like cloaked units etc

    BuildOrderItem item = getTopPriority();
    produceIfPossible(item);

    m_buildingManager.onFrame();
    drawProductionInformation();
}

BuildOrderItem MacroManager::getTopPriority()
{
    std::vector<BuildOrderItem> items;
    for (const auto& manager : m_managers) {
        const auto& prio = manager->getTopPriority();
        if (prio.has_value()) {
            items.push_back(prio.value());
        }
    }

    std::stringstream ss;
    ss << "Production Information\n\n";

    for (auto& item : items)
    {
        ss << item.type.getName() << " : " << item.priority << '\n';
    }

    m_bot.Map().drawTextScreen(0.01f, 0.01f, ss.str(), CCColor(255, 255, 0));

    auto item_ptr = std::max_element(items.begin(), items.end());
    if (item_ptr == items.end()) {
        auto pylonType = UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_bot);
        return BuildOrderItem(MetaType(pylonType, m_bot), 0, false);
    }
    return *item_ptr;
}

void MacroManager::produceIfPossible(BuildOrderItem item) {
    Unit producer = getProducer(item.type);
    bool canMake = canMakeNow(producer, item.type);
    if (producer.isValid() && canMake)
    {
        produce(producer, item);
    }
}

Unit MacroManager::getProducer(const MetaType& type)
{
    // get all the types of units that caa build this type
    auto& producerTypes = m_bot.Data(type).whatBuilds;

    // make a set of all candidate producers
    std::vector<Unit> candidateProducers;
    for (auto unit : m_bot.UnitInfo().getUnits(Players::Self))
    {
        // reasons a unit can not train the desired type
        if (std::find(producerTypes.begin(), producerTypes.end(), unit.getType()) == producerTypes.end()) { continue; }
        if (!unit.isCompleted()) { continue; }
        if (m_bot.Data(unit).isBuilding && unit.isTraining()) { continue; }
        if (unit.isFlying()) { continue; }

        return unit;
    }
    
    // some invalid unit
    return Unit();
}

Unit MacroManager::getProducer(const MetaType & type, CCPosition closestTo)
{
    // get all the types of units that cna build this type
    auto & producerTypes = m_bot.Data(type).whatBuilds;

    // make a set of all candidate producers
    std::vector<Unit> candidateProducers;
    for (auto unit : m_bot.UnitInfo().getUnits(Players::Self))
    {
        // reasons a unit can not train the desired type
        if (std::find(producerTypes.begin(), producerTypes.end(), unit.getType()) == producerTypes.end()) { continue; }
        if (!unit.isCompleted()) { continue; }
        if (m_bot.Data(unit).isBuilding && unit.isTraining()) { continue; }
        if (unit.isFlying()) { continue; }

        // TODO: if unit is not powered continue
        // TODO: if the type is an addon, some special cases
        // TODO: if the type requires an addon and the producer doesn't have one

        // if we haven't cut it, add it to the set of candidates
        candidateProducers.push_back(unit);
    }

    return getClosestUnitToPosition(candidateProducers, closestTo);
}

Unit MacroManager::getClosestUnitToPosition(const std::vector<Unit> & units, CCPosition closestTo)
{
    if (units.size() == 0)
    {
        return Unit();
    }

    // if we don't care where the unit is return the first one we have
    if (closestTo.x == 0 && closestTo.y == 0)
    {
        return units[0];
    }

    Unit closestUnit;
    double minDist = std::numeric_limits<double>::max();

    for (auto & unit : units)
    {
        double distance = Util::Dist(unit, closestTo);
        if (!closestUnit.isValid() || distance < minDist)
        {
            closestUnit = unit;
            minDist = distance;
        }
    }

    return closestUnit;
}

// this function will check to see if all preconditions are met and then create a unit
void MacroManager::produce(const Unit & producer, BuildOrderItem item)
{
    if (!producer.isValid())
    {
        return;
    }

    // if we're dealing with a building
    if (item.type.isBuilding())
    {
        m_buildingManager.addBuildingTask(item.type.getUnitType(), Util::GetTilePosition(m_bot.GetStartLocation()));
    }
    // if we're dealing with a non-building unit
    else if (item.type.isUnit())
    {
        producer.train(item.type.getUnitType());
    }
    else if (item.type.isUpgrade())
    {
        // TODO: UPGRADES
        //Micro::SmartAbility(producer, m_bot.Data(item.type.getUpgradeID()).buildAbility, m_bot);
    }
}

bool MacroManager::canMakeNow(const Unit & producer, const MetaType & type)
{
    if (!producer.isValid() || !meetsReservedResources(type))
    {
        return false;
    }

    sc2::AvailableAbilities available_abilities = m_bot.Query()->GetAbilitiesForUnit(producer.getUnitPtr());

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
    return m_bot.GetMinerals() - m_buildingManager.getReservedMinerals();
}

int MacroManager::getFreeGas()
{
    return m_bot.GetGas() - m_buildingManager.getReservedGas();
}

// return whether or not we meet resources, including building reserves
bool MacroManager::meetsReservedResources(const MetaType & type)
{
    // return whether or not we meet the resources
    int minerals = m_bot.Data(type).mineralCost;
    int gas = m_bot.Data(type).gasCost;

    return (m_bot.Data(type).mineralCost <= getFreeMinerals()) && (m_bot.Data(type).gasCost <= getFreeGas());
}

void MacroManager::drawProductionInformation()
{
}
