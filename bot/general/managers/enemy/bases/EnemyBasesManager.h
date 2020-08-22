#pragma once

#include <general/model/Unit.h>
#include <general/bases/BaseLocationManager.h>

class CCBot;

class EnemyBasesManager {
private:
    CCBot& m_bot;
    std::vector<const Unit*> m_depots;
    // Locations that might be occupied
    std::set<const BaseLocation*> m_expectedBaseLocations;
    // Locations that we know are occupied by depots
    std::map<CCUnitID, const BaseLocation*> m_occupiedBaseLocations;

    void draw();
public:
    EnemyBasesManager(CCBot & bot);

    void onStart();
    void onFrame();
    void newUnitCallback(const Unit* unit);
    void unitDisappearedCallback(const Unit* unit);

    // marks that location might be occupied by enemy
    void expectAsOccupied(const BaseLocation* baseLocation);
    // marks that location is clear (only removes from expected locations)
    void locationIsClear(const BaseLocation* baseLocation);

    const std::vector<const Unit*>& getEnemyDepots() const;
    // returns all expected enemy locations
    const std::vector<const BaseLocation*> getExpectedEnemyBaseLocations() const;
    // returns locations occupied by enemy (excluding expected locations)
    const std::vector<const BaseLocation*> getOccupiedEnemyBaseLocations() const;
    // returns all locations occupied by enemy (including expected locations)
    const std::vector<const BaseLocation*> getAllExpectedEnemyBaseLocations() const;
};
