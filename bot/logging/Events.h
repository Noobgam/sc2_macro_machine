#pragma once

#include <string>
#include <optional>
#include <vector>

class BaseLocation;
class Unit;
class Squad;

namespace events {

    class Event {
    private:
        std::string m_message;
    public:
        Event(std::string message);
        void log();
    };

    namespace enemy_bases_manager {
        Event expectBaseLocationAsOccupied(const BaseLocation* baseLocation, bool result);
        Event expectedBaseLocationIsNotOccupied(const BaseLocation* baseLocation, bool result);
        Event resourceDepotWasDetected(const Unit* unit, std::optional<const BaseLocation*> baseLocation);
        Event resourceDepotIsDisappeared(const Unit* unit, std::optional<const BaseLocation*> baseLocation);
    }

    namespace scout_module {
        Event newScoutSquadFormed(const std::optional<const Squad*>& squad);
        Event setScoutBasesOrder(const Squad* squad, const std::vector<const BaseLocation *> &bases);
        Event scoutSquadWasDeformed(const SquadID id);
        Event scoutSquadIsEmpty(const SquadID id);
    }
}
