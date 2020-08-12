#include <general/managers/enemy/bases/EnemyBasesManager.h>
#include <general/model/Unit.h>

class CCBot;

class EnemyManager {
private:
    CCBot& m_bot;
    EnemyBasesManager m_enemyBases;
public:
    EnemyManager(CCBot &bot);

    EnemyBasesManager& getEnemyBasesManager();
    const EnemyBasesManager& getEnemyBasesManager() const;

    // must be called after BaseLocationManager.onStart()
    void onStart();
    void onFrame();

    void newUnitCallback(const Unit* unit);
    void unitDisappearedCallback(const Unit* unit);
};
