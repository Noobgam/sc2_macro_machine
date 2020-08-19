#include "EnemyManager.h"

EnemyManager::EnemyManager(CCBot &bot)  :
    m_bot(bot),
    m_enemyBases(bot)
{}

EnemyBasesManager& EnemyManager::getEnemyBasesManager() {
    return m_enemyBases;
}

const EnemyBasesManager& EnemyManager::getEnemyBasesManager() const {
    return m_enemyBases;
}

void EnemyManager::onStart() {
    m_enemyBases.onStart();
}

void EnemyManager::onFrame() {
    m_enemyBases.onFrame();
}

void EnemyManager::newUnitCallback(const Unit *unit) {
    m_enemyBases.newUnitCallback(unit);
}

void EnemyManager::unitDisappearedCallback(const Unit *unit) {
    m_enemyBases.unitDisappearedCallback(unit);
}
