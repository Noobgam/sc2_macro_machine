
#include "general/units/UnitInfoManager.h"
#include "general/bases/BaseLocation.h"
#include "micro/squads/Squad.h"
#include <util/LogInfo.h>
#include <sstream>
#include "Events.h"

events::Event::Event(std::string message) : m_message(message) { }

void events::Event::log() {
    LOG_DEBUG << m_message <<  BOT_ENDL;
}

events::Event events::enemy_bases_manager::expectBaseLocationAsOccupied(const BaseLocation *baseLocation, bool result) {
    std::stringstream ss;
    ss << "[ENEMY_BASES_MANAGER] Expecting new base location as occupied " << baseLocation->getBaseId() << ". ";
    ss << "Is new expected location: " << result;
    return events::Event(ss.str());
}

events::Event events::enemy_bases_manager::expectedBaseLocationIsNotOccupied(const BaseLocation* baseLocation, bool result) {
    std::stringstream ss;
    ss << "[ENEMY_BASES_MANAGER] Base location " << baseLocation->getBaseId() << " is clear. ";
    ss << "Expected before: " << result;
    return events::Event(ss.str());
}

events::Event events::enemy_bases_manager::resourceDepotWasDetected(const Unit* unit, std::optional<const BaseLocation*> baseLocation) {
    std::stringstream ss;
    ss << "[ENEMY_BASES_MANAGER] Resource depot " << unit->getID() << " was detected on location ";
    ss << (baseLocation.has_value() ? std::to_string(baseLocation.value()->getBaseId()) : "None");
    return events::Event(ss.str());
}

events::Event events::enemy_bases_manager::resourceDepotIsDisappeared(const Unit* unit, std::optional<const BaseLocation*> baseLocation) {
    std::stringstream ss;
    ss << "[ENEMY_BASES_MANAGER] Resource depot " << unit->getID() << " has disappeared on location ";
    ss << (baseLocation.has_value() ? std::to_string(baseLocation.value()->getBaseId()) : "None");
    return events::Event(ss.str());
}

namespace events::scout_module {
    Event newScoutSquadFormed(const std::optional<const Squad*>& squad) {
        std::stringstream ss;
        ss << "[SCOUT_MODULE] New scout squad was formed: ";
        ss << (squad.has_value() ? std::to_string(squad.value()->getId()) : "None");
        return Event(ss.str());
    }

    Event setScoutBasesOrder(const Squad* squad, const std::vector<const BaseLocation *> &bases) {
        std::stringstream ss;
        ss << "[SCOUT_MODULE] Setting scout order for squad " << squad->getId() << ". Checking bases: ";
        for (const auto& base : bases) {
            ss << base->getBaseId() << " ";
        }
        return Event(ss.str());
    }

    Event scoutSquadWasDeformed(const SquadID id) {
        std::stringstream ss;
        ss << "[SCOUT_MODULE] Scout squad was deformed. Squad id:" << id;
        return Event(ss.str());
    }

    Event scoutSquadIsEmpty(SquadID id) {
        std::stringstream ss;
        ss << "[SCOUT_MODULE] Scout squad is empty: Squad id:" << id;
        return Event(ss.str());
    }
}